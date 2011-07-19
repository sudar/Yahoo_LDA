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
 * eff_small_map.cpp
 *
 *  Created on: 15-Jul-2010
 *      
 */

#include "eff_small_map.h"

simple_map::simple_map(int N) {
    _N = N;
    buckets = new topicCounts*[N + 1];
    for (int i = 0; i < N + 1; i++)
        buckets[i] = new topicCounts(INIT_TC_SIZE);
}

simple_map::~simple_map() {
    if (buckets != NULL) {
        for (int i = 0; i < _N + 1; i++)
            delete buckets[i];
        delete[] buckets;
        buckets = NULL;
    }
}

cnt_t simple_map::get(topic_t key) {
    topicCounts* bucket = buckets[hash(key)];
    return bucket->get_counts(key);
}

void simple_map::put(topic_t key, cnt_t val) {
    topicCounts* bucket = buckets[hash(key)];
    bucket->addNewTop(key, val);
}

void simple_map::clear() {
    for (int i = 0; i < _N + 1; i++)
        buckets[i]->length = 0;
}

std::string simple_map::print() {
    std::stringstream ss;
    for (int i = 0; i < _N + 1; i++) {
        ss << i << ": " << buckets[i]->print() << std::endl;
    }
    return ss.str();
}
