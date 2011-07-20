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
