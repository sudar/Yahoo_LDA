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
