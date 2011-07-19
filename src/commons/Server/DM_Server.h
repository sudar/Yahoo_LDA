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
 * DM_Server.h
 *
 *
 *  Created on: 19-Oct-2010
 *      
 */

#ifndef DM_SERVER_H_
#define DM_SERVER_H_
#include <Ice/Ice.h>
#include "DistributedMap.h"
#include "Hashmap_Array.h"
#include <IceUtil/Mutex.h>
#include <IceUtil/Handle.h>
#include <list>
#include "Server_Helper.h"

using namespace GlobalTable;
using namespace std;

const int QUE_FULL = 50;
const int NUM_CONSUMERS = 3;
const int CHUNK = 10;

/**
 * Container class for a put and get job.
 * The putNget_async operation is an asynchronous operation
 * with asynchronous method dispatch.
 *
 * Hence a call should be queued up for later processing
 * and this class stores all the details pertinent to that call.
 */
class PNGJob: public IceUtil::Shared {
public:
    //!The call back object
    AMD_DistributedMap_putNgetPtr png_cb;

    //!The word on which this operation
    //!is called
    string word;

    //!The serialized form of data that
    //!needs to be accumulated into the
    //!entry pointed by word
    string delta;

    PNGJob(const AMD_DistributedMap_putNgetPtr& cb, const string& word,
            const string& delta) :
        png_cb(cb), word(word), delta(delta) {
    }
};
typedef IceUtil::Handle<PNGJob> PNGJobPtr;


//!The Server class that implements the DistributedMap Ice interface
/**
 *
 * It essentially holds a part of the distributed
 * map. Associates a 'key' to a particular 'value'
 *
 * The implementation abstracts out the semantics.
 * The only accepted key & value types are strings
 * Other types of objects have to be serialized
 * before they can be stored.
 *
 * It provides the usual get, set & remove operations
 * with the usual semantics.
 *
 * It also provides accumulator semantics with the
 * put operation. Values stored using a put operation
 * are accumulated instead of being replaced. Note
 * that the semantics of this accumulating is provided
 * by a helper class that implements the Server_Helper
 * interface
 *
 * There is also a putNget operation which uses AMD to
 * not block dispatch threads. The putNget is also an
 * AMI. More info on AMI in client class. Please look up
 * Ice documentation for more details. This operation puts
 * a value for the given key and returns the result of the
 * accumulate operation.
 *
 * It also provides a barrier implementation that lets
 * clients synchronize. This is a very simple use of AMD
 * The dispatch threads are released without busy waiting.
 * Only a counter is incremented with every call. Once the
 * counter reaches num_clients registered, a response is
 * sent to all queued up call back objects.
 */
class DM_Server: public DistributedMap {
public:
    //!The num of clients that will be accessing the DistributedMap
    //!This parameter is used to provide a barrier
    DM_Server(int num_clients, Server_Helper&);
    virtual ~DM_Server();

    //!The put operation.
    //!Note the slight difference in semantics
    //!This has accumulator semantics. The value is accumulated
    //!and not replaced
    void put(const string& key, const string& val, const Ice::Current&);

    //!The set operation.
    //!The value is replaced if the key already exists
    void set(const string& key, const string& val, const Ice::Current&);

    //!The get operation.
    //!Sets val with the value of the key and returns true
    //!if the key exists. Else does not set val and returns false
    bool get(const string& key, string& val, const Ice::Current&);

    //!The remove operation.
    //!Removes the key-value pair from the table
    //!If the key does not exist, returns false. Else returns true
    bool remove(const string&, string&, const Ice::Current&);

    //!The put and get operation.
    //!Does a put(word,delta) and returns the updated value
    //!png_cb is the AMD call back object
    void putNget_async(const AMD_DistributedMap_putNgetPtr& png_cb,
            const string& word, const string& delta, const Ice::Current&);

    //!Barrier. Helps clients synchronize
    void
            waitForAllClients_async(
                    const AMD_DistributedMap_waitForAllClientsPtr&,
                    const Ice::Current&);

private:
    int _num_clients, _clients_done;
    int _num_consumers, _waiting_thrds;
    typedef vector<AMD_DistributedMap_waitForAllClientsPtr> cb_vector_t;
    cb_vector_t _cbs;
    list<PNGJobPtr> _png_jobs;
    IceUtil::Mutex _cb_mutex, _png_jobs_mutex;
    IceUtil::Monitor<IceUtil::Mutex> _monitor_mutex;

    //!The data structure to store the key-value mapping
    //!Its an array of hashmaps.
    typedef Hashmap_Array<string, string> hashmap_array;
    hashmap_array _tc_table;

    Server_Helper& _server_helper;

    bool get_counts(const string&, string&);
    void upd_counts(const string& word, const string& delta);
};

#endif /* DM_SERVER_H_ */
