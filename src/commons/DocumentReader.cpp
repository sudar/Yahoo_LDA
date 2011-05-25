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
