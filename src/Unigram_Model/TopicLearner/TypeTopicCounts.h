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
 * TypeTopicCounts.h
 * The Main class so to say which maintains the
 * Word-Topic counts table (n(t,w)). This is just a wrapper
 * around the TopicCounts structure that gives a
 * sparse matrix view out of sparse vector(TopicCounts)
 * Also note that the words are not strings but indexes
 * which can be lookedup in the dictionary if needed.
 * The protobuf file also stores the word indexes instead
 * of words to save space
 *
 * It basically has 2 types of initializations:
 * 1. Directly from the words & Topics files (initialize_from_docs) Slow
 * 2. From a binary dump produced by the dump() method (initialize_from_dump) Faster
 *
 * The basic structure maintains the for each unique word
 * found in the corpus/dictionary the number of times
 * each topic has been assigned. It supports multithreaded
 * access with a per word locking granularity. Its designed
 * to do fast updates. The locks are classified into read
 * and write locks to minimize contention. The updates/setters
 * hold a write lock and block further reads and writes on the
 * locked word. The get_counts methods hold a read lock and allow
 * other reads which are way more frequent as there are multiple
 * sampler threads trying to read the topic counts whereas there
 * is only a single thread updating in the single machine case
 * and 2 background threads and the updater thread
 * in the case of multi-machine code. Since update locks are
 * expensive, we try to optimize updates.
 *
 * The basic access pattern is assumed to be:
 * Do get_counts on the reqd word.
 * Update the counts thread locally and
 * Update the shared table.
 *
 * So the basic design aim is to support multi-threaded reads and
 * almost single threaded updates
 *
 * This also maintains the sum of topic counts across the corpus (n(t))
 * However, there is no locking involved while doing this as it is
 * declared as an array of tbb::atomics.
 *
 *  Created on: 24 Mar, 2009
 *      
 */

#ifndef TYPETOPICCOUNTS_H_
#define TYPETOPICCOUNTS_H_

#include <iostream>
#include <exception>

#include <stdio.h>

#include "TopKList.h"
#include "tbb/atomic.h"
#include "TopicCounts.h"
#include "WordIndexDictionary.h"

using namespace std;
using namespace tbb;
using namespace LDA;
using namespace boost;

class TypeTopicCounts {
public:
    /************* Init ************/
    TypeTopicCounts();
    TypeTopicCounts(word_t num_words, topic_t num_topics);
    virtual ~TypeTopicCounts();

    void initialize_from_docs(string wfname, string tfname);
    int verify_header(DocumentReader& doc_rdr);
    void initialize_from_string(word_t word, string& counts);
    bool initialize_from_dump(string fname,
            WordIndexDictionary* local_dict = NULL,
            WordIndexDictionary* global_dict = NULL, size_t offset = 0);
    //    void initialize_from_memcached(const char* servers, int cli_id, int num_clis, WordIndexDictionary* dict, int from, int to);
    void initialize_from_ttc(TypeTopicCounts* ttc);
    static pair<int, float> estimate_fit(string fname,
            WordIndexDictionary* dict);
    static pair<int, float> estimate_fit(string fname, float used_memory,
            int& incoming_words);
    void estimate_alphas(double* alphas, double& alpha_sum);
    void dump(string fname);
    /************* Init Ends ********/

    /************* Getters   ********/
    topic_t get_counts(word_t word, topicCounts* tc);
    topic_t get_counts(atomic<topic_t>* tc);

    word_t get_num_words();
    topic_t get_num_topics();
    word_mutex_t* get_lock(word_t word);
    pair<TopKList**, TopKList*> get_topic_stats();

    //    double model_loglikelihood();
    /************* Getters End********/

    /************* Setters    ********/
    void replace(word_t word, topicCounts& tc);
    void upd_count(word_t word, topic_t old_topic, topic_t new_topic,
            bool ignore_old_topic = false);
    void upd_count(word_t word, mapped_vec delta, string dbg = "");
    /************* Setters End********/

    /************* Test & Debug ********/
    bool equal(const TypeTopicCounts& expected);
    string print(word_t word);
    void print();
    void initialize(topicCounts* wtc, atomic<topic_t>* tc, word_t word = 0);
    void initialize(topicCounts** wtc, atomic<topic_t>* tc);
    /************* Test & Debug Ends ********/

    friend class Ice_Synchronizer;
    friend class Memcached_Synchronizer;

protected:
    atomic<topic_t> *tokens_per_topic; //n(t)
    topic_t num_topics; // The number of topics being learnt
    TopKList **topic_stats, //Topic Statistics per topic
            *top_topics; //The hot/top topics


    void estimate_memoryn_warn(long num_elems);
    void clear_stats();
    void init(topic_t num_topics_);
    void destroy();

private:
    topicCounts **wt; //The pointer to and array of topicCounts one per word
    word_mutex_t *word_mutexes; //An array of locks one per word
    word_t num_words; //The num of unique words

    bool validate();
};

#endif /* TYPETOPICCOUNTS_H_ */
