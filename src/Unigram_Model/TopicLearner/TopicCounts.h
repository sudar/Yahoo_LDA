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
