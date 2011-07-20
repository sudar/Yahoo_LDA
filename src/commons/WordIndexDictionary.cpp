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
 * WordIndexDictionary.cpp
 *
 *  Created on: 7 May, 2009
 *      
 */

#include "WordIndexDictionary.h"
#include <iostream>
#include "document.pb.h"
#include "DocumentWriter.h"
#include "glog/logging.h"
#include <sstream>
#include "comparator.h"

using namespace LDA;

/**
 * Constructs an empty dictionary
 */
WordIndexDictionary::WordIndexDictionary() {
    current_index = 0;
    wi_end = word_ind_map.end();
    string suffices_[100] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
            "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31",
            "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42",
            "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53",
            "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64",
            "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75",
            "76", "77", "78", "79", "80", "81", "82", "83", "84", "85", "86",
            "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97",
            "98", "99" };
    for (int i = 0; i < 100; i++)
        suffices[i] = suffices_[i];
}

WordIndexDictionary::~WordIndexDictionary() {
}

/**
 * inserts a word with the given index if it
 * does not already exist in the dictionary.
 * The only point to take care is to update
 * the iterator wi_end whenever a new insertio
 * happens. This is done to save computing this
 * every check to see if word exists in dictionary.
 */
int WordIndexDictionary::insert_word(string word, int index_) {
    wimap::iterator it = word_ind_map.find(word);
    if (it != wi_end)
        return (*it).second;

    int& index = word_ind_map[word];
    wi_end = word_ind_map.end();
    index = index_;
    pair<int, string> p(index, word);
    ind_word_map.insert(p);
    return -1;
}

/**
 * Find the unique index assigned to word
 */
int WordIndexDictionary::get_index(string word) {
    if (word_ind_map.count(word) == 0)
        return -1;
    return word_ind_map[word];
}

/**
 * Find the word having index as its index
 */
string WordIndexDictionary::get_word(int index) {
    string empty = "";
    if (ind_word_map.count(index) == 0)
        return empty;
    return ind_word_map[index];
}

/**
 * Insert the word into the dictionary if it doesn't exist
 * This automatically manages assigning unique indices
 */
int WordIndexDictionary::insert_word(string word) {
    int retVal = insert_word(word, current_index);
    if (retVal == -1) {
        id2freq_t freq_ent(current_index, 1);
        frequencies.push_back(freq_ent);
        return current_index++;
    }
    ++frequencies[retVal].second;
    return retVal;
}

int WordIndexDictionary::get_freq(int index) {
    return frequencies[index].second;
}

/**
 * Dumps the dictionary onto disk in protobuffer
 * binary format so that a new dictionary can be
 * intialized later from the dump using initialize_from_dump
 * Also the dump does batch write to disk to optimize
 * io. Batches 1000 (word,index) pairs and then writes
 * them to disk using DocumentWriter
 */
void WordIndexDictionary::dump(string fname) {
    DocumentWriter doc_wrtr(fname);
    header hdr;
    hdr.set_version(CURRENT_VERSION);
    hdr.set_num_words(get_num_words());
    doc_wrtr.write(hdr);
    word_ind_pair wipair;
    int i = 0;
    for (wimap::iterator beg = word_ind_map.begin(); beg != word_ind_map.end(); beg++) {
        wipair.add_word((*beg).first);
        int cur_id = (*beg).second;
        wipair.add_index(cur_id);
        wipair.add_frequency(frequencies[cur_id].second);
        if (++i % 1000 == 0) {
            LOG_IF(FATAL,!doc_wrtr.write(wipair))<<"Couldn't write to the stream while dumping WordIndexDictionary. Quitting";
            wipair.Clear();
        }
    }
    if(wipair.word_size()>0) {
        LOG_IF(FATAL,!doc_wrtr.write(wipair)) << "Couldn't write to the stream while dumping WordIndexDictionary. Quitting";
        wipair.Clear();
    }
}

int WordIndexDictionary::verify_header(DocumentReader& doc_rdr) {
    header* hdr = new header;
    doc_rdr.read(hdr);
    double incoming_ver = hdr->version();
    LOG_IF(FATAL,incoming_ver > CURRENT_VERSION)<< "Version mismatch. File Version: " << incoming_ver << " LDA Version: " << CURRENT_VERSION;
    int incoming_words = -1;
    if(hdr->has_num_words()) incoming_words = hdr->num_words();
    delete hdr;
    return incoming_words;
}

void WordIndexDictionary::initialize_from_dict(WordIndexDictionary* dict,
        bool sort) {
    word_ind_map.clear();
    ind_word_map.clear();
    frequencies.clear();
    frequencies.reserve(dict->get_num_words());
    id2freq_t freq_ent(0, 0);
    for (int i = 0; i < dict->get_num_words(); i++) {
        frequencies.push_back(freq_ent);
    }
    int max_ind = -1;
    wimap::iterator end = dict->word_ind_map.end();
    for (wimap::iterator beg = dict->word_ind_map.begin(); beg != end; beg++) {
        string word = (*beg).first;
        int index = (*beg).second;

        insert_word(word, index);
        int freq = dict->frequencies[index].second;
        id2freq_t freq_ent(index, freq);
        frequencies[index] = freq_ent;
        if (index > max_ind)
            max_ind = index;
    }
    current_index = max_ind + 1;
    if (sort) {
        sort_on_freq();
    }
}

/**
 * Initializes from a dump file produced by dump
 * Reads the (word,index) pairs from the file &
 * populates the maps
 */
void WordIndexDictionary::initialize_from_dump(string fname, int num_words,
        bool sort) {
    word_ind_map.clear();
    ind_word_map.clear();
    frequencies.clear();
    DocumentReader doc_rdr(fname);
    int incoming_words = verify_header(doc_rdr);
    frequencies.reserve(min(incoming_words, num_words));
    id2freq_t freq_ent(0, 0);
    for (int i = 0; i < min(incoming_words, num_words); i++) {
        frequencies.push_back(freq_ent);
    }
    word_ind_pair* wipair = new word_ind_pair;
    int max_ind = -1;
    while (doc_rdr.read(wipair) != -1) {
        for (int i = 0; i < wipair->word_size(); i++) {
            string word = wipair->word(i);
            int index = wipair->index(i);
            if (index >= num_words)
                continue;

            insert_word(word, index);
            int freq = wipair->frequency(i);
            id2freq_t freq_ent(index, freq);
            frequencies[index] = freq_ent;
            if (index > max_ind)
                max_ind = index;
        }
    }
    LOG_IF(WARNING,incoming_words!=-1 && (max_ind+1) != incoming_words)<<"Num of words in the dump file header(" << incoming_words << ") is different from that read (" << (max_ind+1) << ")";
    current_index = max_ind + 1;
    delete wipair;
    if(sort)
    sort_on_freq();
}

string WordIndexDictionary::get_suffix(int n) {
    if (n < 100)
        return suffices[n];
    stringstream ss;
    ss << n;
    return ss.str();
}

int WordIndexDictionary::get_prev_index(int new_id) {
    return frequencies[new_id].first;
}

void WordIndexDictionary::sort_on_freq() {
    std::sort(frequencies.begin(), frequencies.end(), freq_cmp);
    for (int i = 0; i < frequencies.size(); i++) {
        int from = frequencies[i].first;
        string w_from = get_word(from);
        word_ind_map[w_from] = i;
        //        frequencies[i].first = i;
    }
    wimap::iterator end = word_ind_map.end();
    for (wimap::iterator beg = word_ind_map.begin(); beg != end; beg++) {
        ind_word_map[(*beg).second] = (*beg).first;
    }
}

void WordIndexDictionary::initialize_from_dumps(string prefix, int dictionaries) {
    word_ind_map.clear();
    ind_word_map.clear();
    frequencies.clear();
    int global_index = 0;
    word_ind_pair* wipair = new word_ind_pair;
    for (int n = 0; n < dictionaries; n++) {
        DocumentReader doc_rdr(prefix + "." + get_suffix(n));
        verify_header(doc_rdr);
        while (doc_rdr.read(wipair) != -1) {
            for (int i = 0; i < wipair->word_size(); i++) {
                int retVal = insert_word(wipair->word(i), global_index);
                int cur_freq = wipair->frequency(i);
                if (retVal == -1) {
                    id2freq_t freq_ent(global_index, cur_freq);
                    frequencies.push_back(freq_ent);
                    ++global_index;
                } else
                    frequencies[retVal].second += cur_freq;

            }
        }
    }
    delete wipair;
    sort_on_freq();
}

//Return the number of unique words that have been inserted
int WordIndexDictionary::get_num_words() const {
    return word_ind_map.size();
}

/**
 * This is a method aiding testing. This tests for the
 * uniqueness of indices. It also does this by making
 * the assumption that the indices have to sequential
 * and reduces the complexity of testing uniqueness by
 * checking the actual sum of the indices assigned to
 * the expected sum comuted as sigma(last_index_assigned)
 * Should always return true. If you change the logic
 * for assigning unique indices, make sure you modify
 * this method to verify it.
 */
bool WordIndexDictionary::match_word_index() {
    int sum = 0;
    for (wimap::iterator beg = word_ind_map.begin(); beg != word_ind_map.end(); beg++) {
        int ind = get_index((*beg).first);
        sum += ind;
        string word = get_word(ind);
        if ((*beg).first != word)
            return false;
    }
    int num_uniq = get_num_words();
    int exp_sum = num_uniq * (num_uniq - 1) / 2;
    if (sum != exp_sum) {
        LOG(INFO) << "Indices not unique. sum=" << sum << " exp sum="
                << exp_sum << endl;
        return false;
    }
    return true;
}

//Retuns the total number of elements which is twice
//get_num_words
size_t WordIndexDictionary::size() {
    return word_ind_map.size() + ind_word_map.size();
}

/**
 * Log the dictionary to log(INFO)
 */
void WordIndexDictionary::print() {
    LOG(INFO) << "Num words:" << get_num_words();
    std::stringstream ss;
    for (wimap::iterator beg = word_ind_map.begin(); beg != word_ind_map.end(); beg++) {
        int cur_id = (*beg).second;
        ss << (*beg).first << ":" << cur_id << ":"
                << frequencies[cur_id].second << endl;
    }
    cout << ss.str();
    ss.str("");
}
