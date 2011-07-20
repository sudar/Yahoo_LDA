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
 * Unigram_Model_Server_Helper.h
 *
 *  Created on: 07-Jan-2011
 *      
 */

#ifndef UNIGRAM_MODEL_SERVER_HELPER_H_
#define UNIGRAM_MODEL_SERVER_HELPER_H_

#include "Server/Server_Helper.h"
#include "types.h"

class Unigram_Model_Server_Helper: public Server_Helper {
public:
    Unigram_Model_Server_Helper();
    virtual ~Unigram_Model_Server_Helper();

    void combine(const string entity, const string& old, const string& delta,
            string& combined);
};

#endif /* UNIGRAM_MODEL_SERVER_HELPER_H_ */
