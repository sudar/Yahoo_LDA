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
