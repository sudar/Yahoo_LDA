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
 * Unigram_Model.cpp
 *
 *  Created on: 05-Jan-2011
 *      
 */

#include "Unigram_Model.h"
#include "Context.h"
#include "glog/logging.h"
#include "TopicLearner/Dirichlet.h"

Unigram_Model::Unigram_Model(int num_words, int num_topics) {
    _ttc = new TypeTopicCounts(num_words, num_topics);
    top_words_per_topic = new topK_word_prop_t*[num_topics];
    for (int i = 0; i < num_topics; i++) {
        top_words_per_topic[i] = new topK_word_prop_t(NUM_WORDS_PER_TOP);
    }
    _top_words_empty = true;
}

Unigram_Model::~Unigram_Model() {
    delete _ttc;
    int num_topics = Context::get_instance().get_int("topics");
    for (int i = 0; i < num_topics; i++)
        delete top_words_per_topic[i];
    delete[] top_words_per_topic;
}

Parameter& Unigram_Model::get_parameter(int which) {
    if (which == Unigram_Model::ALPHA)
        return _alpha;
    else if (which == Unigram_Model::BETA)
        return _beta;
    else
        LOG(FATAL) << "Unknown parameter being asked from Unigram Model";
}

void Unigram_Model::set_parameter(int which, Parameter& param) {
    if (which == Unigram_Model::ALPHA)
        _alpha = param;
    else if (which == Unigram_Model::BETA)
        _beta = param;
    else
        LOG(FATAL) << "Unknown parameter being asked from Unigram Model";
}

TypeTopicCounts& Unigram_Model::get_ttc() {
    return *_ttc;
}

double Unigram_Model::get_eval() {
    int nonZeroTypeTopics = 0;
    int non_zero_topics = 0;
    double loglikelihood = 0.0;
    double avgLen = 0.0;
    int num_topics = _ttc->get_num_topics();
    int num_words = _ttc->get_num_words();
    tbb::atomic<int> n_t[num_topics];
    _ttc->get_counts(n_t);
    float beta = _beta.values[0];
    float beta_bar = _beta.sum;
    for (int t = 0; t < num_topics; t++) {
        int nt = n_t[t];
        if (nt != 0) {
            ++non_zero_topics;
            loglikelihood -= log_gamma(beta_bar + nt);
        }
        top_words_per_topic[t]->clear();
        for (int w = 0; w < num_words; w++) {
            wppair wp(w, beta / (beta_bar + nt));
            top_words_per_topic[t]->push(wp);
        }
    }
    topicCounts tc(num_topics);
    for (int w = 0; w < num_words; w++) {
        _ttc->get_counts(w, &tc);
        int length = tc.length;
        nonZeroTypeTopics += length;
        avgLen += length;
        for (int j = 0; j < length; j++) {
            cnt_topic_t cnt_top = tc.items[j];
            int t = cnt_top.choose.top;
            int n_tw = cnt_top.choose.cnt;
            float phi_tw = (beta + n_tw) / (beta_bar + n_t[t]);
            wppair wp(w, phi_tw);
            top_words_per_topic[t]->push(wp);
            loglikelihood += log_gamma(beta + n_tw);
        }
    }
    avgLen /= num_words;
    LOG(WARNING) << "Average num of topics assigned per word = " << avgLen;

    loglikelihood += non_zero_topics * log_gamma(beta_bar)
            - nonZeroTypeTopics * log_gamma(beta);

    _top_words_empty = false;
    return loglikelihood;
}

bool Unigram_Model::save() {
    Context& context = Context::get_instance();
    string ttc_dump = context.get_string("ttcdump");
    string param_dump = context.get_string("paramdump");
    LOG(WARNING) << "Saving model for test pipeline in " << ttc_dump
            << " and " << param_dump;
    _ttc->dump(ttc_dump);
    _alpha.dump(param_dump);
    LOG(WARNING) << "Model saved";
    return true;
}

void Unigram_Model::write_statistics(WordIndexDictionary& dict) {
    if (_top_words_empty)
        get_eval();
    int num_topics = _ttc->get_num_topics();
    Context& context = Context::get_instance();
    string tw_out = context.get_string("tw_out");
    LOG(WARNING) << "Writing top words identified per topic into "
            << tw_out;
    ofstream out(tw_out.c_str());
    LOG_IF(FATAL,!out.is_open())<< "Unable to open output file: " << tw_out;
    for(int i=0;i<num_topics;i++) {
        out << "Topic " << i << ": ";
        stack<wppair> st;
        double sum = 0.0;
        while(!top_words_per_topic[i]->empty()) {
            wppair wp = top_words_per_topic[i]->top();
            sum += wp.second;
            st.push(wp);
            top_words_per_topic[i]->pop();
        }
        while(!st.empty()) {
            wppair wp = st.top();
            out << "(" << dict.get_word(wp.first) << "," << wp.second/sum <<") ";
			st.pop();
		}
		out << endl;
	}
	out.flush();
	out.close();
	LOG(WARNING) << "Word statistics per topic written";
}
