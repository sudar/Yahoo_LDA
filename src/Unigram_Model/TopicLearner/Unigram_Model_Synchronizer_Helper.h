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
 * Unigram_Model_Synchronizer_Helper.h
 *
 *  Created on: 13-Jan-2011
 *      
 */

#ifndef UNIGRAM_MODEL_SYNCHRONIZER_HELPER_H_
#define UNIGRAM_MODEL_SYNCHRONIZER_HELPER_H_

#include "TopicLearner/Synchronizer_Helper.h"
#include "TypeTopicCounts.h"
#include "WordIndexDictionary.h"
#include "Client.h"

class Unigram_Model_Synchronizer_Helper: public Synchronizer_Helper {
public:
    Unigram_Model_Synchronizer_Helper(TypeTopicCounts& ttc,
            WordIndexDictionary& dict);
    virtual ~Unigram_Model_Synchronizer_Helper();

    void initialize();
    bool has_to_synchronize();
    void reset_to_synchronize();
    void synchronize();
    void end_putNget(const std::string& word, const std::string& counts);

private:
    TypeTopicCounts& _ttc;
    WordIndexDictionary& _dict;

    TypeTopicCounts* _prev_ttc;
    Client* _client;

    int _num_words, _num_topics;

    int _cur_word;

    topicCounts tc_client, tc_old;
};

#endif /* UNIGRAM_MODEL_SYNCHRONIZER_HELPER_H_ */
