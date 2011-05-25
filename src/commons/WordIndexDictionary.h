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
