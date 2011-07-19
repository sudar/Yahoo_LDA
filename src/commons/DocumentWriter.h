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
 * DocumentWriter.h
 *
 *  Created on: 8 May, 2009
 *      
 */

#ifndef DOCUMENTWRITER_H_
#define DOCUMENTWRITER_H_

#include <fstream>
#include "google/protobuf/message.h"
#include "constants.h"

using namespace std;
using namespace LDA;

/**
 * Wrapper around protobuf msgs for convenient
 * writing of words, topics & (word,index) pairs
 * into word, topic, dictionary dump files respectively.
 * Each msg is written into a binary file in record* format
 * where record=(size of serialized msg,msg serialized as string)
 */
class DocumentWriter {
public:
    DocumentWriter(string w_fname_);
    virtual ~DocumentWriter();
    bool write(const google::protobuf::Message& msg);

private:
    string w_fname; //The output file to write various msgs from

    ofstream w_output; //The output stream to write to w_fname

    string serialized; //The string to which msgs are serialized to

    /**
     * The main function that writes records to output
     * Assumes that the msg has been serialized by the
     * caller into 'seialized'
     */
    inline bool write_sized_record_to(ofstream& output) {
        LOG_IF(FATAL,serialized.size()>size_t(MAX_MSG_SIZE))<<"Writing file: Message size " << serialized.size() << " exceeds " << MAX_MSG_SIZE << ". Quitting...";

        size_int size = (size_int)serialized.size();
        output.write((char*)&size,sizeof(size_int));
        output.write(serialized.c_str(),size);
        output.flush();
        return !output.bad();
    }

    //The base method to write msg into out_str
    inline bool write_base(ofstream& out_str, const google::protobuf::Message& msg) {
        msg.SerializeToString(&serialized);
        return write_sized_record_to(out_str);
    }
};

#endif /* DOCUMENTWRITER_H_ */
