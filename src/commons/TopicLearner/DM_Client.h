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
 * DM_Client.h
 *
 *
 *
 *  Created on: 19-Oct-2010
 *      
 */

#ifndef DM_CLIENT_H_
#define DM_CLIENT_H_

#include <Ice/Ice.h>
#include <vector>
#include "Server/DistributedMap.h"
#include <IceUtil/Handle.h>
#include "TopicLearner/Synchronizer_Helper.h"
#include "Client.h"
#include "boost/unordered_map.hpp"
#include "constants.h"

using namespace std;
using namespace GlobalTable;

struct Cookie: public Ice::LocalObject {
    string entity;
    int msg_id;
};
typedef IceUtil::Handle<Cookie> CookiePtr;

//!The call back object for the Put and Get AMI.
/**
 * Ice calls the finished method once the AMI completes. The
 * client is not tied to any particular call back. The actual
 * call back is provided by a helper class, the sync_helper
 * whose end method is called to complete the full AMI call.
 *
 * This call back class also doubles up as a rate limiter
 * implementing a sliding window mechanism for sending
 * messages to the server.
 *
 * Given a max number of msgs, it creates an array to track
 * the status of the msgs that have been sent to the server
 *
 * It provides a monitor based wait mechanism for the client
 * which can wait till the server responds back to a particular
 * msg. We take the liberty of making a fcfs assumption on the
 * server side processing for keeping things simple.
 */
class PNGCallback: public IceUtil::Shared {
public:
    //!The sync_helper and the max num of msgs to track
    PNGCallback(Synchronizer_Helper& sync_helper, int num_msgs) :
        _sync_helper(sync_helper), _num_msgs(num_msgs) {
        _num_synchs = 0;
        _waiting = -1;
        _status = new bool[_num_msgs];
        for (int i = 0; i < _num_msgs; i++) {
            _status[i] = true;
        }
    }

    ~PNGCallback() {
        delete[] _status;
    }

    //!Wait till the server responds to msg with id msg_id
    bool wait_till_done(int msg_id) {
        if (_status[msg_id])
            return true;
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
        while (!_status[msg_id]) {
            try {
                _waiting = msg_id;
                _monitor.wait();
                _waiting = -1;
            } catch (const Ice::Exception& ex) {
                _waiting = -1;
                throw ex;
            }
        }
        return true;
    }

    //!set or clear the status flag for a particular msg_id
    void set_done(int msg_id, bool status) {
        _status[msg_id] = status;
    }

    //!Provides the number of completed AMI callbacks
    int num_synchs() {
        return _num_synchs;
    }

    /**
     * This method is called by Ice run time when the server
     * responds. We do some bookkeeping and delegate the
     * task of actually completing the AMI by calling
     * sync_helper.end method
     */
    void finished(const Ice::AsyncResultPtr& r) {
        DistributedMapPrx dist_map = DistributedMapPrx::uncheckedCast(
                r->getProxy());
        string global_cnts;
        CookiePtr cookie = CookiePtr::dynamicCast(r->getCookie());
        int msg_id = cookie->msg_id;
        string entity = cookie->entity;
        try {
            dist_map->end_putNget(global_cnts, r);
        } catch (const Ice::Exception& ex) {
            throw ex;
        }
        _sync_helper.end_putNget(entity, global_cnts);
        ++_num_synchs;
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
        _status[msg_id] = true;
        if (_waiting == msg_id) {
            _monitor.notify();
        }
    }

private:
    Synchronizer_Helper& _sync_helper;
    int _num_msgs, _num_synchs;
    bool *_status;
    int _waiting;
    IceUtil::Monitor<IceUtil::Mutex> _monitor;
};
typedef IceUtil::Handle<PNGCallback> PNGCallbackPtr;

//!The client used to access the Distributed Map
/**
 * Wrapper class to delegate the Distributed Map
 * operations to the appropriate Server
 */
class DM_Client: public Client {
public:
     /**
       * The servers in the Distributed Map setup
       * The max number of entities that you are going to
       * store in the map. This is just to make sure that
       * things dont break when you are processing a very small
       * number of entities particularly smaller than MAX_MSGS
       * The helper class that provides the actual call back functionality
       */
    DM_Client(int num_entities, const string& servers,
            Synchronizer_Helper& sync_helper);
    virtual ~DM_Client();

    //!The usual map operations
    void put(const string& entity, const string& delta);
    void set(const string& entity, const string& counts);
    bool get(const string& entity, string& counts);
    bool remove(const string& entity, string& counts);

    //!Start the asynchronous putNget operation
    void begin_putNget(const string& entity, const string& delta);

    //!Execute a barrier
    void wait_for_all();

    void wait_till_done();

    int get_num_servers();

private:
    void add_server(const string& servant_name, const string& server_endpoint);
    inline size_t get_server(const string& s);

private:
    Ice::CommunicatorPtr _ic;
    vector<DistributedMapPrx> _dist_maps;
    Ice::CallbackPtr _cb;
    PNGCallbackPtr _png_cb;
    int NUM_SERVERS, _num_entities;
    boost::hash<string> _hasher;
    size_t _cur_msg_id, _max_msgs;
};

#endif /* DM_CLIENT_H_ */
