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
 * DocumentWriter.cpp
 *
 *  Created on: 8 May, 2009
 *      
 */

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif /* O_LARGEFILE */

#include "DocumentWriter.h"

/**
 * Constructs a DocumentWriter object to write msgs
 * into w_fname and optionally topics to t_fname.
 * You can however choose to ignore the words file
 * if you want to write only topics by setting
 * w_fname_="" & using write_topics() method.
 */
DocumentWriter::DocumentWriter(string w_fname_) {
    w_fname = w_fname_;
    w_output.open(w_fname.c_str(), ios::out | ios::trunc | ios::binary);
    LOG_IF(FATAL,w_fname!="" && !w_output.is_open())<<"Unable to open output file: " << w_fname;
}

DocumentWriter::~DocumentWriter() {
    w_output.close();
}

/**
 * The default method to write a message to w_fname
 * in (size of serialized msg,msg serialized as string)*
 * format
 */
bool DocumentWriter::write(const google::protobuf::Message& msg) {
    return write_base(w_output, msg);
}
