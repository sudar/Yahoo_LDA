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
 * Unigram_Model.h
 *
 *  Created on: 05-Jan-2011
 *      
 */

#ifndef UNIGRAM_MODEL_H_
#define UNIGRAM_MODEL_H_

#include "TopicLearner/Model.h"
#include <string>
#include "TopicLearner/Parameter.h"
#include "TypeTopicCounts.h"
#include "TopicLearner/GenericTopKList.h"

using namespace std;

class Unigram_Model: public Model {
public:
    const static int ALPHA = 1;
    const static int BETA = 2;
public:
    Unigram_Model(int, int);
    virtual ~Unigram_Model();

    Parameter& get_parameter(int);
    void set_parameter(int, Parameter&);
    TypeTopicCounts& get_ttc();

    double get_eval();

    bool save();

    void write_statistics(WordIndexDictionary&);

private:
    TypeTopicCounts* _ttc;
    param _alpha, _beta;
    typedef GenericTopKList<wppair, wppair_gt> topK_word_prop_t;
    topK_word_prop_t** top_words_per_topic;
    bool _top_words_empty;
};

#endif /* UNIGRAM_MODEL_H_ */
