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
