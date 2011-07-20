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
 * Unigram_Train_Data_Formatter.cpp
 *
 *  Created on: 11-Jan-2011
 *      
 */

#include "Unigram_Train_Data_Formatter.h"
#include "glog/logging.h"
#include "commons/Context.h"
#include "commons/document.pb.h"
#include "commons/constants.h"

Unigram_Train_Data_Formatter::Unigram_Train_Data_Formatter() {
    _num_docs = 0;
    _num_words_in_all_docs = 0;
    using namespace boost;
    //Read the stop words into a set
    stringstream ss;
    for (int i = 0; i < NUM_STP_WRDS; i++) {
        ss << stopwords[i] << " : ";
        _stopWords.insert(stopwords[i]);
    }
    LOG(INFO) << ss.str();
    Context& context = Context::get_instance();
    string output_w = context.get_string("output_w");
    if (output_w != "")
        _doc_writer = new DocumentWriter(output_w);

    string corpus = context.get_string("corpus");
    if (corpus != "") {
        _in.open(corpus.c_str());
        LOG_IF(FATAL,!_in.is_open())<< "Unable to open input file: " << corpus <<endl;
    }
}

Unigram_Train_Data_Formatter::~Unigram_Train_Data_Formatter() {
    delete _doc_writer;
}

void Unigram_Train_Data_Formatter::format() {
    std::istream& in =
            (Context::get_instance().get_string("corpus") == "") ? std::cin
                    : _in;
    int st_wrd_cnt = 0;
    LDA::unigram_document wdoc;
    while (true) {
        int retVal = read_from_inp(wdoc, in);
        if (retVal == -1)
            break;
        st_wrd_cnt += retVal;
        if ((_num_docs + 1) % 50000 == 0) {
            LOG(WARNING) << "Read doc " << (_num_docs + 1);
            LOG(INFO) << "Dictionary built with " << (_num_docs + 1)
                    << " lines of input.";
            LOG(INFO) << "Dictionary size: " << _dict.size();
        }
        int num_words_cur_doc = wdoc.body_size();
        if (num_words_cur_doc > 0) {
            ++_num_docs;
            _num_words_in_all_docs += num_words_cur_doc;
            _doc_writer->write(wdoc);
        }
    }
    LOG(INFO) << "Found " << st_wrd_cnt << " stop words" << endl;
}

int Unigram_Train_Data_Formatter::read_from_inp(LDA::unigram_document& wdoc,
        std::istream& inp) {
    string word;
    string line;
    if (getline(inp, line).eof()) {
        LOG_IF(FATAL,inp.bad())<< "Unable to read from word input file";
        return -1;
    }
    int st_wrd_cnt=0;
    int skip = 0;
    wdoc.Clear();
    char *line_ind = (char*)(line.c_str());
    bool more = true;

    while(more) {
        char *wrd = line_ind;
        word.clear();

        while(*line_ind!=' ' && *line_ind!=0) {
            if(skip>1) {
                *line_ind = tolower(*line_ind);
            }
            word.push_back(*line_ind);
            ++line_ind;
        }

        if(*line_ind==0) {
            //Blank line. Get new line
            if(wrd==line_ind)
            break;
            //All but the last word processed
            //Process the last and don't enter
            else
            more = false;
        }
        else
        ++line_ind;

        //Skip first two entries storing the first
        //as docid
        if (skip++ < 2) {
            if(skip==1)
            wdoc.set_docid(word);
            else if(skip==2) {
                wdoc.set_url(word);
            }
            continue;
        }
        //Skip stop words
        if (_stopWords.count(word) > 0) {
            ++st_wrd_cnt;
            continue;
        }

        //Insert into dictionary if needed
        //Else check if it exists in the supplied one
        int windex = insert_word_to_dict(word);
        if(windex>=0)
        wdoc.add_body(windex);
        else
        ++st_wrd_cnt;
    }
    return st_wrd_cnt;
}

int Unigram_Train_Data_Formatter::insert_word_to_dict(string word) {
    return _dict.insert_word(word);
}

int Unigram_Train_Data_Formatter::get_num_docs() {
    return _num_docs;
}

int Unigram_Train_Data_Formatter::get_total_num_words() {
    return _num_words_in_all_docs;
}

WordIndexDictionary& Unigram_Train_Data_Formatter::get_dictionary() {
    return _dict;
}
