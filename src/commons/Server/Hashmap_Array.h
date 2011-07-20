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
 * Hashmap_Array.h
 *
 *
 *  Created on: 04-Aug-2010
 *      
 */

#ifndef HASHMAP_ARRAY_H_
#define HASHMAP_ARRAY_H_
#include "boost/unordered_map.hpp"
#include "types.h"
#include "tbb/spin_rw_mutex.h"

using namespace boost;
using namespace std;
using namespace tbb;

/**
 * An array of hash maps.
 * Reduces Contention on multi-threaded access.
 */
template<class Key, class Value>
class Hashmap_Array {
public:
    Hashmap_Array() {
    }
    virtual ~Hashmap_Array() {
    }
    typedef unordered_map<Key, Value> act_map;
    typedef typename unordered_map<Key, Value>::iterator act_map_iter;

    size_t size() const {
        size_t size = 0;
        for (int i = 0; i < NUM_MAPS; i++)
            size += maps[i].size();
        return size;
    }

    size_t count(Key key) const {
        register int ind = hasher(key) & MASK;
        const act_map& map = maps[ind];
        return map.count(key);
    }

    Value& operator[](Key key) {
        register int ind = hasher(key) & MASK;
        act_map& map = maps[ind];
        return map[key];
    }

    void erase(Key key) {
        register int ind = hasher(key) & MASK;
        act_map& map = maps[ind];
        map.erase(key);
    }

    word_mutex_t* get_lock(Key key) {
        register int ind = hasher(key) & MASK;
        return &rw_mutexes[ind];
    }

    word_mutex_t* get_structure_lock() {
        return &structure_mutex;
    }

    /**
     * The iterator is not thread safe.
     */
    class iterator {
    private:
        act_map_iter current_end;
        int current_ind;
        act_map* bigram_counts;

    public:
        act_map_iter current_iter;

        iterator(int ind, act_map_iter iter, act_map_iter end, act_map* bc) {
            current_iter = iter;
            current_ind = ind;
            current_end = end;
            bigram_counts = bc;
        }

        void operator++(int) {
            current_iter++;
            if (current_iter == current_end && current_ind != NUM_MAPS - 1) {
                ++current_ind;
                for (; current_ind < NUM_MAPS; current_ind++) {
                    current_iter = bigram_counts[current_ind].begin();
                    current_end = bigram_counts[current_ind].end();
                    if (current_iter != current_end)
                        break;
                }
            }
        }

        bool operator!=(iterator& inp) {
            return (current_iter != inp.current_iter);
        }
    };

    iterator begin() {
        int i = 0;
        for (; i < NUM_MAPS; i++) {
            if (maps[i].begin() != maps[i].end())
                break;
        }

        if (i == NUM_MAPS) {
            //empty
            iterator beg(i, maps[NUM_MAPS - 1].begin(),
                    maps[NUM_MAPS - 1].end(), maps);
            return beg;
        } else {
            iterator beg(i, maps[i].begin(), maps[i].end(), maps);
            return beg;
        }

    }

    iterator end() {
        iterator end(NUM_MAPS - 1, maps[NUM_MAPS - 1].end(),
                maps[NUM_MAPS - 1].end(), maps);
        return end;
    }
private:
    const static int NUM_MAPS = 1024;
    const static int MASK = NUM_MAPS - 1;
    act_map maps[NUM_MAPS];
    word_mutex_t rw_mutexes[NUM_MAPS];
    word_mutex_t structure_mutex;
    hash<Key> hasher;
};

#endif /* HASHMAP_ARRAY_H_ */
