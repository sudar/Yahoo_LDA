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
 * Server_Helper.h
 *
 *
 *
 *  Created on: 07-Jan-2011
 *      
 */

#ifndef SERVER_HELPER_H_
#define SERVER_HELPER_H_

#include <string>

using namespace std;



//! Server Helper interface
/**
 * Users of the Distributed Map need to provide
 * semantics for the put operation using this
 * helper class.
 *
 * The put operation calls helper.combine()
 * to perform the accumulate operation
 *
 * All server helpers must implement this interface
 */
class Server_Helper {
public:
    virtual void combine(const string entity, const string& old,
            const string& delta, string& combined) = 0;
};

#endif /* SERVER_HELPER_H_ */
