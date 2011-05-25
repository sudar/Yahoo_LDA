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
 * DocumentReader.h
 *
 *
 *  Created on: 7 May, 2009
 *      
 */

#ifndef DOCUMENTREADER_H_
#define DOCUMENTREADER_H_

#include <fstream>
#include "google/protobuf/message.h"
#include "constants.h"

using namespace std;
using namespace LDA;

/**
 * Wrapper around protobuf messages for convenient
 * reading of words, topics & (word,index) pairs
 * from word, topic, dictionary dump files respectively.
 * Assumes that each msg is in a binary file in record* format
 * where record=(size of serialized msg,msg serialized as string)
 */
class DocumentReader {
public:
    DocumentReader(string w_fname_);
    virtual ~DocumentReader();

    int read(google::protobuf::Message* msg);

private:

    string w_fname; //The input file to read various msgs from

    ifstream w_input; //The input stream to read from w_fname

    char* c_size; //Array to store size of serialized msg
    char* c_msg; //Array to store the serialized msg

    /**
     * The main function that reads records from input and stores them c_size & c_msg
     * fname is used for logging
     */
    inline int read_sized_record_from(ifstream& input, string fname)
            throw (int) {
        if (input.read(c_size, sizeof(size_int)).eof()) {
            LOG_IF(FATAL,input.bad())<< "Unable to read from input file: " <<fname;
            throw -1;
        }
        size_int size = *(size_int*)c_size;
        LOG_IF(FATAL,size>=MAX_MSG_SIZE) << "Reading input from " << fname << "Message size " << size << " exceeds " << MAX_MSG_SIZE << ". Quitting...";

        input.read(c_msg,size);
        return size;
    }

    /**
     * 	The base method to read into msg from inp_str
     *	inp_fname is used for logging
     */
    inline void read_base(ifstream& inp_str, string& inp_fname, google::protobuf::Message* msg) throw(int) {
        size_int size = read_sized_record_from(inp_str, inp_fname);
        string str_message(c_msg, size);
        msg->Clear();
        msg->ParseFromString(str_message);
    }
};

#endif /* DOCUMENTREADER_H_ */
