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
 * types.h
 * Contains various type definitions used globally
 * throughout the code
 *
 *  Created on: 25 Mar, 2009
 *      
 */

#ifndef TYPES_H_
#define TYPES_H_
#include <vector>

#include "tbb/spin_rw_mutex.h"
#include "document.pb.h"
#include "tbb/tick_count.h"
#include <sstream>
#include <boost/random/mersenne_twister.hpp>
#include "glog/logging.h"
#include "boost/unordered_map.hpp"
#include "tbb/atomic.h"

#define TIME(t) tick_count t = tick_count::now()
#define PRINT_TIME(t1,t2,str) cout << "Time taken to " #str << " : " << (t2-t1).seconds() << " secs" << endl

typedef int32_t size_int; //A type to denote the size of serialize protobuf msgs

typedef int32_t cnt_t; //A type for counts
typedef int32_t topic_t; //A type for topics
typedef int32_t word_t; //A type for words
typedef uint64_t packed_t; //A type for the packed
//(topic,counts)

static const uint16_t INIT_TC_SIZE = 8; //Initial block allocation for TopicCounts
static const uint16_t SUBSEQ_ALLOCS = 4; //Subsequent allocations for TopicCounts

static const float MEM_LOAD_PER = 0.7;

typedef std::pair<int, int> bigram_key_t;

/*typedef struct{
 float* values;
 int length;
 float sum;
 }Parameters;*/

typedef struct {
    //A structure to conveniently
    //and portably access topics
    //and counts separately. We try
    //to avoid direct access when
    //speed is not a major concern
    topic_t top;
    cnt_t cnt;
} choose_t;

typedef union {
    //The main structure for the storage
    //and access of (topic,counts) in a
    //packed fashion.
    //Example: (100,10) is stored in pkd form
    //using cnt_top_t ct; ct.choose.top=100;
    //ct.choose.cnt=10; Access is similar.
    //This is the only portable way of doing
    //it. The moment you start accessing portions
    //of cnt_top directly, you make assumptions
    //on the order of struct storage and endianness.
    //There are parts in sampler & TopicCounts which are
    //totally arch specific and are not portable
    //This was done to improve performance
    packed_t cnt_top;
    choose_t choose;
} cnt_topic_t;

typedef tbb::spin_rw_mutex word_mutex_t; //A type for read,write spin lock

typedef struct {
    //Message type to store the
    //changes suggested by sampler
    word_t word;
    topic_t old_topic;
    topic_t new_topic;
    int old_indicator;
    int new_indicator;
} change_elem_t;

typedef struct {
    //Message type between sampler
    //and updater
    LDA::unigram_document* doc;
    std::vector<change_elem_t>* change_list;
    bool ignore_old_topic;
} update_t;

typedef boost::mt19937 base_generator_type; //variate generator used by RNGS

typedef std::pair<topic_t, float> tppair; //(topic,proportion) pair used in dumping
//topic proportions to disk

typedef std::pair<word_t, float> wppair; //(word,proportion) pair
typedef std::pair<bigram_key_t, float> bigppair; //(bigram,proportion) pair

typedef std::pair<int, int> id2freq_t; //(word_id,frequency) used in dictionary to compute
//and maintain word frequencies

/**
 * Exception thrown when an old topic is not found
 * This is fatal and causes the program to stop
 */
class InvalidOldTopicExc: public std::exception {
    word_t word;
    topic_t old_topic;

public:
    InvalidOldTopicExc(word_t word_, topic_t old_topic_) {
        word = word_;
        old_topic = old_topic_;
    }

    virtual const char* what() const throw () {
        std::ostringstream ret;
        ret << "The old topic " << old_topic
                << " argument is invalid for the word " << word;
        return ret.str().c_str();
    }
};

typedef boost::unordered_map<topic_t, cnt_t> mapped_vec; //The type for map used in topicCounts

#endif /* TYPES_H_ */
