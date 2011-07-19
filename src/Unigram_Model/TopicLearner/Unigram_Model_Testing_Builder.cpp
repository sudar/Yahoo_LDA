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
 * Unigram_Model_Builder.cpp
 *
 *  Created on: 06-Jan-2011
 *      
 */

#include "Unigram_Model_Testing_Builder.h"
#include "Context.h"
#include "DocumentReader.h"
#include "DocumentWriter.h"
#include "Unigram_Model_Tester.h"
#include "TopicLearner/TBB_Pipeline.h"
#include "TopicLearner/Testing_Execution_Strategy.h"

Unigram_Model_Testing_Builder::Unigram_Model_Testing_Builder() {
}

Unigram_Model_Testing_Builder::~Unigram_Model_Testing_Builder() {
}

Model_Refiner& Unigram_Model_Testing_Builder::create_model_refiner() {
    Context& context = Context::get_instance();
    string input_prefix = context.get_string("inputprefix");
    string input_w = input_prefix + ".wor";
    int num_topics = context.get_int("topics");

    context.put_string("input_w", input_w);

    //The following is needed so that the create_output
    //method which is not overridded works. It uses input_t
    //as the topic assignments file and in testing we do
    //not need that. It also deletes the output_t file.
    //So we switch back to creating a tmp out file
    string input_t = input_prefix + ".top";
    context.put_string("input_t", input_t);
    string output_t = input_prefix + ".top.tmp";
    context.put_string("output_t", output_t);

    string ttc_dumpfile = context.get_string("dumpprefix") + ".ttc.dump";
    string param_dumpfile = context.get_string("dumpprefix") + ".par.dump";
    string global_dict_dump = context.get_string("inputprefix")
            + ".dict.dump.global";

    context.put_string("ttc_dumpfile", ttc_dumpfile);
    context.put_string("param_dumpfile", param_dumpfile);
    context.put_string("global_dict_dump", global_dict_dump);

    TypeTopicCounts& ttc = _model->get_ttc();
    Parameter& alpha = _model->get_parameter(Unigram_Model::ALPHA);
    Parameter& beta = _model->get_parameter(Unigram_Model::BETA);

    _refiner = new Unigram_Model_Tester(ttc, alpha, beta, get_dict());
    return *_refiner;
}

Execution_Strategy& Unigram_Model_Testing_Builder::create_execution_strategy(
        Pipeline& pipeline) {
    _strategy = new Testing_Execution_Strategy(pipeline, *_model);
    return *_strategy;
}
