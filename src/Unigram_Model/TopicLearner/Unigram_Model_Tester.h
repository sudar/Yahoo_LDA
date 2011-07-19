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
 * Unigram_Model_Tester.h
 *
 *  Created on: 06-Jan-2011
 *      
 */

#ifndef UNIGRAM_MODEL_TESTER_H_
#define UNIGRAM_MODEL_TESTER_H_

#include "TopicLearner/Model_Refiner.h"
#include "TypeTopicCounts.h"
#include "TopicLearner/Parameter.h"
#include "DocumentReader.h"
#include "DocumentWriter.h"
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>
#include "WordIndexDictionary.h"

using namespace boost;
using namespace std;

class Unigram_Model_Tester: public Model_Refiner {
public:
    Unigram_Model_Tester(TypeTopicCounts&, Parameter&, Parameter&,
            WordIndexDictionary&, bool no_init = false);
    virtual ~Unigram_Model_Tester();

    google::protobuf::Message* allocate_document_buffer(size_t);
    void deallocate_document_buffer(google::protobuf::Message*);
    google::protobuf::Message* get_nth_document(
            google::protobuf::Message* docs, size_t n);
    void* read(google::protobuf::Message&);
    void* sample(void*);
    void* update(void*);
    void* optimize(void*);
    void* eval(void*, double&);
    void write(void*);
    void iteration_done();

    void* test(void*);

    static long doc_index; //Running count of all the documents processed by the optimizer

private:
    void set_up_io(string, string);
    void release_io();

protected:
    TypeTopicCounts& _ttc;
    Parameter& _alpha;
    Parameter& _beta;
    bool ignore_old_topic;
    int _num_words, _num_topics;
    //Reader
    DocumentReader *_wdoc_rdr;
    DocumentWriter *_tdoc_writer;
};

#endif /* UNIGRAM_MODEL_TESTER_H_ */
