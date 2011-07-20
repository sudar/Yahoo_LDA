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
 * Unigram_Train_Data_Formatter.h
 *
 *  Created on: 11-Jan-2011
 *      
 */

#ifndef UNIGRAM_TRAIN_DATA_FORMATTER_H_
#define UNIGRAM_TRAIN_DATA_FORMATTER_H_

#include "commons/Formatter/Data_Formatter.h"
#include "commons/DocumentWriter.h"
#include <fstream>
#include "boost/unordered_set.hpp"

class Unigram_Train_Data_Formatter: public Data_Formatter {
public:
    Unigram_Train_Data_Formatter();
    virtual ~Unigram_Train_Data_Formatter();

    void format();

    WordIndexDictionary& get_dictionary();

    int get_num_docs();

    int get_total_num_words();

protected:
    virtual int insert_word_to_dict(std::string word);
    int read_from_inp(LDA::unigram_document & wdoc, std::istream& inp);

protected:
    WordIndexDictionary _dict;
    int _num_docs, _num_words_in_all_docs;
    boost::unordered_set<string> _stopWords;
    std::ifstream _in;
    DocumentWriter* _doc_writer;
};

#endif /* UNIGRAM_TRAIN_DATA_FORMATTER_H_ */
