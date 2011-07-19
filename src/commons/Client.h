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
 * Client.h
 *
 *
 *
 *  Created on: 07-Jan-2011
 *      
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>

using namespace std;

/**
 * The interface to be implemented by any client for the
 * Distributed Map service
 */
class Client {
public:
    //!Gets the serialized val stored for the key in
    //!the Distributed Map. Returns true if key exists
    //!and false otherwise
    virtual bool get(const string& key, string& val) = 0;

    //!Sets val as the serialized value for the key in
    //!the Distributed Map. This has replace semantics
    virtual void set(const string& key, const string& val) = 0;

    //!Adds delta to the serialized value for the key in
    //!the Distributed Map. This has accumulator semantics.
    //!The accumulator logic is provided by the Server_Helper.
    virtual void put(const string& key, const string& delta) = 0;

    //!Asynchronous put and get operation. It begins with
    //!a put. The call back in Synchronizer_Helper is called
    //!which creates the effect of a get
    virtual void begin_putNget(const string& key, const string& val) = 0;

    //!Remove the key from the Distributed Map returning
    //!the serialized value stored as val. Returns true if key exists
    //!and false otherwise
    virtual bool remove(const string& key, string& val) = 0;

    //!Provides barrier functionality
    virtual void wait_for_all() = 0;

    //!Provides functionality to wait for any asynchronous communication
    //!to end
    virtual void wait_till_done() = 0;
};

#endif /* CLIENT_H_ */
