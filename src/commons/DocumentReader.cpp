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
 * DocumentReader.cpp
 *
 *  Created on: 7 May, 2009
 *      
 */

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif /* O_LARGEFILE */

#include "DocumentReader.h"

/**
 * Constructs a DocumentReader object to read msgs
 * from w_fname and optionally topics from t_fname
 */
DocumentReader::DocumentReader(string w_fname_) {
    w_fname = w_fname_;
    w_input.open(w_fname.c_str(), ios::in | ios::binary);
    LOG_IF(FATAL,w_fname!="" && !w_input.is_open())<<"Unable to open input file: " << w_fname;

    c_size = new char [sizeof(size_int)];
    c_msg = new char [MAX_MSG_SIZE];
}

DocumentReader::~DocumentReader() {
    w_input.close();
    delete[] c_size;
    delete[] c_msg;
}

/**
 * The default method to read a message from w_fname which
 * is in (size of serialized msg,msg serialized as string)*
 * format
 */
int DocumentReader::read(google::protobuf::Message* msg) {
    try {
        read_base(w_input, w_fname, msg);
        return 0;
    } catch (int e) {
        return -1;
    }
}
