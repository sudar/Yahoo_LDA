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
