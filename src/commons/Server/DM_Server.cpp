/*******************************************************************************
    Copyright (c) 2011 Yahoo! Inc. All rights reserved.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License. See accompanying LICENSE file.

    The Initial Developer of the Original Code is Shravan Narayanamurthy.
******************************************************************************/
/*
 * DM_Server.cpp
 *
 *  Created on: 19-Oct-2010
 *      
 */

#include "DM_Server.h"
#include "LDAUtil.h"
#include "tbb/atomic.h"
#include "TopicLearner/Model.h"
#include "Server/Unigram_Model_Server_Helper.h"

DM_Server::DM_Server(int num_clients, Server_Helper& server_helper) :
    _num_clients(num_clients), _server_helper(server_helper) {
    _clients_done = 0;
    _num_consumers = 0;
    _waiting_thrds = 0;
}

DM_Server::~DM_Server() {
}

void DM_Server::upd_counts(const string& word, const string& delta) {
    string& old = _tc_table[word];
    _server_helper.combine(word, old, delta, old);
}

bool DM_Server::get_counts(const string& word, string& counts) {
    if (_tc_table.count(word) == 0)
        return false;
    counts.assign(_tc_table[word]);
    return true;
}

void DM_Server::put(const string& word, const string& delta,
        const Ice::Current&) {
    word_mutex_t::scoped_lock lock(*(_tc_table.get_lock(word)), true);
    upd_counts(word, delta);
}

void DM_Server::set(const string& word, const string& counts,
        const Ice::Current&) {
    word_mutex_t::scoped_lock lock(*(_tc_table.get_lock(word)), true);
    _tc_table[word].assign(counts.c_str(), counts.size());
}

bool DM_Server::get(const string& word, string& counts, const Ice::Current&) {
    word_mutex_t::scoped_lock lock(*(_tc_table.get_lock(word)), false);
    return get_counts(word, counts);
}

bool DM_Server::remove(const string& word, string& counts, const Ice::Current&) {
    word_mutex_t::scoped_lock lock(*(_tc_table.get_lock(word)), false);
    if (!get_counts(word, counts))
        return false;
    _tc_table.erase(word);
    return true;
}

void DM_Server::putNget_async(const AMD_DistributedMap_putNgetPtr& png_cb,
        const string& word, const string& delta, const Ice::Current&) {
    IceUtil::Monitor<IceUtil::Mutex>::Lock monitor(_monitor_mutex);
    PNGJobPtr job = new PNGJob(png_cb, word, delta);
    _png_jobs.push_back(job);
    if (_num_consumers >= NUM_CONSUMERS) {
        //cerr << _num_consumers << " are working" << endl;
        int num_jobs_to_respond = _png_jobs.size();
        if (num_jobs_to_respond < QUE_FULL) {
            //cerr << "Queue is not full. Queuing up the job. Que Size: " << num_jobs_to_respond << endl;
            if (_waiting_thrds) { /*cerr << "Waking up waiting consumers" << endl;*/
                _monitor_mutex.notify();
            }
        } else {
            //cerr << "Queue is full. Acting as consumer" << endl;
            num_jobs_to_respond = min(CHUNK, (int) _png_jobs.size());
            vector<PNGJobPtr> my_png_jobs;
            for (int i = 0; i < num_jobs_to_respond; i++) {
                my_png_jobs.push_back(_png_jobs.front());
                _png_jobs.pop_front();
            }
            //cerr << "Cleared " << CHUNK << " jobs to process. Que Size: " << _png_jobs.size() << endl;
            monitor.release();
            for (int i = 0; i < num_jobs_to_respond; i++) {
                PNGJobPtr my_job = my_png_jobs[i];
                word_mutex_t::scoped_lock lock(*(_tc_table.get_lock(
                        my_job->word)), true);
                upd_counts(my_job->word, my_job->delta);
                string counts;
                get_counts(my_job->word, counts);
                lock.release();
                my_job->png_cb->ice_response(counts);
                //cerr << "Sent Response to job " << i << endl;
            }
            my_png_jobs.clear();
        }
    } else {
        //cerr << _num_consumers << " are working" << endl;
        ++_num_consumers;
        vector<PNGJobPtr> my_png_jobs;
        while (true) {
            int num_jobs_to_respond = min(CHUNK, (int) _png_jobs.size());
            for (int i = 0; i < num_jobs_to_respond; i++) {
                my_png_jobs.push_back(_png_jobs.front());
                _png_jobs.pop_front();
            }
            //cerr << "Cleared " << num_jobs_to_respond << " jobs to process. Que Size: " << _png_jobs.size() << endl;
            monitor.release();
            for (int i = 0; i < num_jobs_to_respond; i++) {
                PNGJobPtr my_job = my_png_jobs[i];
                word_mutex_t::scoped_lock lock(*(_tc_table.get_lock(
                        my_job->word)), true);
                upd_counts(my_job->word, my_job->delta);
                string counts;
                get_counts(my_job->word, counts);
                lock.release();
                my_job->png_cb->ice_response(counts);
                //cerr << "Sent Response to job " << i << endl;
            }
            my_png_jobs.clear();
            monitor.acquire();
            while (_png_jobs.empty()) {
                try {
                    ++_waiting_thrds;
                    //cerr << "Queue empty. " << _waiting_thrds << " are waiting including me" << endl;
                    _monitor_mutex.wait();
                    --_waiting_thrds;
                    //cerr << "Woke up, " << _waiting_thrds << " are waiting" << endl;
                } catch (const Ice::Exception& ex) {
                    --_waiting_thrds;
                    throw ex;
                }
            }
        }
    }
}

void DM_Server::waitForAllClients_async(
        const AMD_DistributedMap_waitForAllClientsPtr& cb, const Ice::Current&) {
    IceUtil::Mutex::Lock scoped_lock(_cb_mutex);
    ++_clients_done;
    _cbs.push_back(cb);
    if (_clients_done == _num_clients) {
        _clients_done = 0;
        cb_vector_t::iterator it = _cbs.begin();
        for (; it != _cbs.end(); ++it)
            (*it)->ice_response();
        _cbs.clear();
    }
}

int main(int argc, char* argv[]) {
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        if (argc < 5 || 5 < argc)
            throw "Incorrect arguments\n"
            "Take a look at docs/html/index.html for more info.\n\n";
        int model = atoi(argv[1]);
        int server_id = atoi(argv[2]);
        int num_clients = atoi(argv[3]);
        string host_port(argv[4]);
        cerr << "Attaching DM_Server_Adapter to endpoint "
                << LDAUtil::DM_Server_Names::get_server_endpoint(host_port)
                << endl;
        Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints(
                "DM_Server_Adapter",
                LDAUtil::DM_Server_Names::get_server_endpoint(host_port));

        Server_Helper* helper = NULL;
        if (model == Model::UNIGRAM)
            helper = new Unigram_Model_Server_Helper;
        Ice::ObjectPtr object = new DM_Server(num_clients, *helper);
        cerr << "Adding servant with name "
                << LDAUtil::DM_Server_Names::get_servant_name(server_id)
                << endl;
        adapter->add(object, ic->stringToIdentity(
                LDAUtil::DM_Server_Names::get_servant_name(server_id)));
        adapter->activate();
        ic->waitForShutdown();
        delete helper;
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception& e) {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}
