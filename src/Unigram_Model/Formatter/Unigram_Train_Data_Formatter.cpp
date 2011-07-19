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
