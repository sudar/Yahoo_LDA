/*****************************************************************************
     The contents of this file are subject to the Mozilla Public License
     Version 1.1 (the "License"); you may not use this file except in
     compliance with the License. You may obtain a copy of the License at
     http://www.mozilla.org/MPL/

     Software distributed under the License is distributed on an "AS IS"
     basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
     License for the specific language governing rights and limitations
     under the License.

     The Original Code is Copyright (C) by Yahoo! Research.

     The Initial Developer of the Original Code is Shravan Narayanamurthy.

     All Rights Reserved.
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
