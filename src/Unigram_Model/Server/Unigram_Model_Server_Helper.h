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
