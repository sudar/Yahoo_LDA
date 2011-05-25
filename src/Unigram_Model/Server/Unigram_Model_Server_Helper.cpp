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
 * Unigram_Model_Server_Helper.cpp
 *
 *  Created on: 13-Jan-2011
 *      
 */

#include "Unigram_Model_Server_Helper.h"
#include "Unigram_Model/TopicLearner/TopicCounts.h"

Unigram_Model_Server_Helper::Unigram_Model_Server_Helper() {
}

Unigram_Model_Server_Helper::~Unigram_Model_Server_Helper() {
}

void Unigram_Model_Server_Helper::combine(const string entity,
        const string& old, const string& delta, string& combined) {
    mapped_vec delta_map;

    topicCounts tc_old(old);
    topicCounts tc_delta(delta);

    tc_old += tc_delta;

    tc_old.convertTo(combined);
}
