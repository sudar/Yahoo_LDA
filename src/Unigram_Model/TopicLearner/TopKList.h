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
 * TopKList.h
 * A list that maintains top K records
 * Each record is assumed to be the
 * cnt_topic_t union. The abstraction
 * of the record is (word,cnt) pair
 * So top K records indicate K unique
 * records having the highest count
 *
 * The main assumption is that the
 * records inserted should be unique
 * wrt to the words. Same word with
 * different counts should not be
 * inserted. Results are undefined
 *
 *  Created on: 14 May, 2009
 *      
 */

#ifndef TOPKLIST_H_
#define TOPKLIST_H_
#include "constants.h"
#include <algorithm>
#include "comparator.h"

typedef cnt_topic_t cnt_word_t;

class TopKList {
public:
    TopKList(int K_);
    virtual ~TopKList();
    //Assumes that the words inserted are unique and doesn't check this explicitly
    void insert_word(const cnt_word_t& cnt_word);
    bool is_sorted();
    cnt_word_t get_max();
    typedef cnt_topic_t* iterator;
    iterator get_beg();
    iterator get_end();
    void print();
    void clear();

private:
    int K, //The val K in top K
            num_elements; //The actual num of elements stored; can be lesser than K

    cnt_word_t* array; //The array which stores the elements. Its always kept sorted

    int32_t min; //The min count. Used to chk if an incoming element should be inserted

    void insert_into_array(const cnt_word_t& cnt_word);
};

#endif /* TOPKLIST_H_ */
