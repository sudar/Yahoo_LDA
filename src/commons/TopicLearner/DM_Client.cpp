/*******************************************************************************
    Copyright (c) 2011, Yahoo! Inc.
    All rights reserved.

    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:

    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.

    * Neither the name of Yahoo! Inc. nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of Yahoo! Inc.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
