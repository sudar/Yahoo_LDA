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
 * WordIndexDictionary.h
 *
 *
 *
 *  Created on: 7 May, 2009
 *      
 */

#ifndef WORDINDEXDICTIONARY_H_
#define WORDINDEXDICTIONARY_H_

#include "boost/unordered_map.hpp"
#include "types.h"
#include "DocumentReader.h"
#include <climits>

using namespace std;
using namespace boost;
//!A two way dictionary of words to indices
/**
 * Provides a two way dictionary mapping
 * words as strings to a unique int index
 * and vice versa. The hashtable implementation
 * of boost/unordered_map is used.
 *
 */
class WordIndexDictionary {
public:
    WordIndexDictionary();
    virtual ~WordIndexDictionary();
    int get_index(string word);
    string get_word(int index);
    int insert_word(string word);
    int get_num_words() const;
    void print();
    bool match_word_index();
    void dump(string fname);
    void initialize_from_dict(WordIndexDictionary* dict, bool sort = false);
    void initialize_from_dump(string fname, int num_words = INT_MAX, bool sort =
            false);
    void initialize_from_dumps(string prefix, int dumps);
    size_t size();
    int get_prev_index(int new_id);
    int get_freq(int index);

    vector<id2freq_t> frequencies;

private:
    typedef unordered_map<string, int> wimap;
    unordered_map<string, int> word_ind_map;
    unordered_map<int, string> ind_word_map;

    wimap::iterator wi_end;
    int current_index;
    int insert_word(string word, int index_);
    int verify_header(DocumentReader & doc_rdr);
    string get_suffix(int n);
    void sort_on_freq();
    string suffices[100];
};

#endif /* WORDINDEXDICTIONARY_H_ */
