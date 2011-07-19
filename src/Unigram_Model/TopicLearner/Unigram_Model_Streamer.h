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
 * Unigram_Model_Streamer.h
 *
 *  Created on: 28-Mar-2011
 *      
 */

#ifndef UNIGRAM_MODEL_STREAMER_H_
#define UNIGRAM_MODEL_STREAMER_H_

#include "Formatter/Unigram_Train_Data_Formatter.h"
#include "Unigram_Model_Tester.h"

class Unigram_Model_Streamer: public Unigram_Train_Data_Formatter,
        public Unigram_Model_Tester {
public:
    Unigram_Model_Streamer(TypeTopicCounts&, Parameter&, Parameter&,
            WordIndexDictionary&, WordIndexDictionary&);
    virtual ~Unigram_Model_Streamer();
    void* read(google::protobuf::Message&);
    void write(void*);
    void iteration_done();

protected:
    int insert_word_to_dict(std::string word);

private:
    WordIndexDictionary& _local_dict;
};

#endif /* UNIGRAM_MODEL_STREAMER_H_ */
