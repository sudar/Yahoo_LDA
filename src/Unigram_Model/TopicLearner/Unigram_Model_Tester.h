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
