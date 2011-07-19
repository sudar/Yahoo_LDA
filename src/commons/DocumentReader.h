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
