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
