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
