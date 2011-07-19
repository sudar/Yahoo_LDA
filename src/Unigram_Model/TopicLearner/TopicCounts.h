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
 * TopicCounts.h
 * The main structure that stores the sparse
 * topic counts vector. It uses a blocked
 * allocation scheme in that memory is allocated
 * and deallocated in blocks. Blocked allocation
 * is used to reduce heap fragmentation.
 *
 * The elements are of the type cnt_topic_t
 * which packs both the topic and the count
 * into a single 64bit integer.
 *
 * Supports a map-view of the sparse vector,
 * but you don't have order information in
 * this view. So use with discretion.
 *
 * Its a struct for speed. So everything is
 * public. Don't mess with the items, length
 * and origLenth if you aren't sure about
 * what you are doing. Use the methods
 * provided making sure the assumptions hold
 *
 *  Created on: 14 Oct, 2009
 *      
 */

#ifndef TOPICCOUNTS_H_
#define TOPICCOUNTS_H_

#include "types.h"
#include <algorithm>
#include "boost/unordered_map.hpp"
#include "tbb/atomic.h"
#include "comparator.h"
#include <vector>

class simple_map;

typedef struct TopicCounts {
    cnt_topic_t* items; //The actual array holding data
    //which is dynamically reshaped
    //This is always sorted in descending
    //order and only has non-zero entries
    //Methods do not check for uniqueness
    //but assume uniqueness.
    //Responsiblity of user to ensure
    //uniqueness

    topic_t length; //The number of elements stored in the array

    topic_t origLength; //The size of the allocated array

    std::vector<cnt_topic_t> vec_items;

    //mapped_vec tmp_map; //A temporary map to hasten some
    //internal update operations
    int frequency;
    bool QUIT;

    /***** Init *****/
    TopicCounts();
    TopicCounts(int length);
    TopicCounts(cnt_topic_t* it, int len);
    TopicCounts(const std::string& counts);
    void init(cnt_topic_t* it, int len);
    void init(const std::string& counts);
    ~TopicCounts();
    void assign(int length, bool setLen = true);
    void setLength(int length_);
    /***** Init *****/
    /***** Getters *****/
    void findOldnNew(topic_t oldTopic, topic_t newTopic, topic_t** oldTop,
            topic_t** newTop);
    int get_frequency();
    cnt_t get_counts(topic_t topic);
    int convertTo(mapped_vec& map, int mult = 1) const;
    void convertTo(simple_map& map, int mult = 1) const;
    void convertTo(std::string& counts) const;
    int convertTo_d(mapped_vec& map, double mult) const;
    //int computeFrequency();
    //bool matchFrequency();
    /***** Getters *****/

    /***** Setters *****/
    bool findAndIncrement(topic_t topic);
    bool findAndDecrement(topic_t topic);
    //void setFrequency();

    void compact();

    void addNewTop(topic_t topic, cnt_t count = 1);
    void addNewTopAftChk(topic_t topic, cnt_t count = 1);
    void upd_count(mapped_vec& delta, tbb::atomic<topic_t>* t = NULL);

    //Convenience updates
    void operator+=(TopicCounts& inp);
    void operator-=(TopicCounts& inp);

    void removeOldTop(topic_t ind, cnt_topic_t& ct);

    void replace(TopicCounts& tc);

    void decrement(topic_t ind, topic_t** newTop);
    void increment(topic_t ind);
    /***** Setters *****/

    //Test & Debug
    std::string print();
    TopicCounts(mapped_vec& map);
    bool equal(const TopicCounts& expected);
} topicCounts;

#endif /* TOPICCOUNTS_H_ */
