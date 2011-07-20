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
 * DM_Client.cpp
 *
 *  Created on: 19-Oct-2010
 *      
 */

#include "DM_Client.h"
#include "LDAUtil.h"
#include <algorithm>
#include "boost/unordered_map.hpp"

DM_Client::DM_Client(int num_entities, const string& servers,
        Synchronizer_Helper& sync_helper) {
    _num_entities = num_entities;
    _max_msgs = min(_num_entities, MAX_MSGS);
    _cur_msg_id = 0;
    _png_cb = new PNGCallback(sync_helper, _max_msgs);
    vector<string> tokens;
    LDAUtil::StringTokenizer::tokenize(servers, ',', tokens);
    for (size_t i = 0; i < tokens.size(); ++i) {
        add_server(LDAUtil::DM_Server_Names::get_servant_name(i),
                LDAUtil::DM_Server_Names::get_server_endpoint(tokens[i]));
    }
    _cb = Ice::newCallback(_png_cb, &PNGCallback::finished);
}

DM_Client::~DM_Client() {
    if (_ic)
        _ic->destroy();
}

void DM_Client::add_server(const string& servant_name,
        const string& server_endpoint) {
    try {
        if (!_ic) {
            int argc = 2;
            char* argv[2] = { (char*) "learnTopics",
                    (char*) "--Ice.ThreadPool.Client.SizeMax=2" };
            _ic = Ice::initialize(argc, argv);
        }
        string proxy(servant_name + ":" + server_endpoint);
        Ice::ObjectPrx base = _ic->stringToProxy(proxy);
        DistributedMapPrx dist_map = DistributedMapPrx::checkedCast(base);
        if (!dist_map)
            throw "Invalid Proxy: " + proxy;
        _dist_maps.push_back(dist_map);
        NUM_SERVERS = _dist_maps.size();
    } catch (const Ice::Exception& ex) {
        cerr << ex << endl;
        throw ex;
    } catch (const char* msg) {
        cerr << msg << endl;
        throw msg;
    }
}

int DM_Client::get_num_servers() {
    return NUM_SERVERS;
}

size_t DM_Client::get_server(const string& s) {
    return _hasher(s) % NUM_SERVERS;
}

void DM_Client::begin_putNget(const string& entity, const string& delta) {
    _png_cb->wait_till_done(_cur_msg_id);
    CookiePtr cookie = new Cookie;
    cookie->entity = entity;
    cookie->msg_id = _cur_msg_id;
    _png_cb->set_done(_cur_msg_id, false);
    _dist_maps[get_server(entity)]->begin_putNget(entity, delta, _cb, cookie);
    if (++_cur_msg_id % _max_msgs == 0) {
        wait_till_done();
    }
}

void DM_Client::wait_till_done() {
    if (_cur_msg_id == 0)
        return;
    _png_cb->wait_till_done(_cur_msg_id - 1);
    _cur_msg_id = 0;
}

void DM_Client::put(const string& entity, const string& delta) {
    _dist_maps[get_server(entity)]->put(entity, delta);
}

void DM_Client::set(const string& entity, const string& counts) {
    _dist_maps[get_server(entity)]->set(entity, counts);
}

bool DM_Client::get(const string& entity, string& counts) {
    return _dist_maps[get_server(entity)]->get(entity, counts);
}

bool DM_Client::remove(const string& entity, string& counts) {
    return _dist_maps[get_server(entity)]->remove(entity, counts);
}

void DM_Client::wait_for_all() {
    _dist_maps[0]->waitForAllClients();
}
