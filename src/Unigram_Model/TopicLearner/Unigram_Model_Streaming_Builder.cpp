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
 * Unigram_Model_Streaming_Builder.cpp
 *
 *  Created on: 29-Mar-2011
 *      
 */

#include "Unigram_Model_Streaming_Builder.h"
#include "Context.h"
#include "Unigram_Model_Streamer.h"
#include "TopicLearner/TBB_Pipeline.h"
#include "TopicLearner/Testing_Execution_Strategy.h"

Unigram_Model_Streaming_Builder::Unigram_Model_Streaming_Builder() {
    _dict = NULL;
    Context& context = Context::get_instance();
    string dict_dump = context.get_string("dictionary");
    context.put_string("dictdump", dict_dump);

    LOG(WARNING) << "Initializing global dictionary from " << dict_dump;
    _global_dict = new WordIndexDictionary;
    _global_dict->initialize_from_dump(dict_dump);
    LOG(WARNING) << "Dictionary initialized and has "
            << _global_dict->get_num_words();

    WordIndexDictionary& dict = get_dict();

    int num_words = dict.get_num_words();
    int num_topics = context.get_int("topics");

    _model = new Unigram_Model(num_words, num_topics);
}

Unigram_Model_Streaming_Builder::~Unigram_Model_Streaming_Builder() {
    delete _dict;
    delete _refiner;
    delete _pipeline;
    delete _strategy;
    delete _model;
}

void Unigram_Model_Streaming_Builder::init_dict() {
    _dict = new WordIndexDictionary;
    Context& context = Context::get_instance();
    string dict_dump = Context::get_instance().get_string("dictdump");
    string ttc_dumpfile = context.get_string("dumpprefix") + ".ttc.dump";
    int num_dumps = context.get_int("numdumps");
    int maxmemory = context.get_int("maxmemory");
    if (num_dumps == 1) {
        LOG(WARNING) << "Estimating the words that will fit in "
                << maxmemory << " MB";
        pair<int, float> wordsnmem = TypeTopicCounts::estimate_fit(
                ttc_dumpfile, _global_dict);
        int num_words = wordsnmem.first;
        LOG(WARNING) << num_words << " will fit in " << wordsnmem.second
                << " MB of memory";
        LOG(WARNING) << "Initializing Local Dictionary from " << dict_dump
                << " with " << num_words << " words.";
        _dict->initialize_from_dump(dict_dump, num_words, true);
    } else {
        float used_mem = 0.0f;
        int num_words = 0;
        stringstream ss;
        LOG(WARNING) << "Estimating the words that will fit in "
                << maxmemory << " MB";
        for (int dump = 0; dump < num_dumps; dump++) {
            ss << ttc_dumpfile;
            ss << "." << dump;
            string dumpfile = ss.str();
            int incoming_words;
            pair<int, float> wordsnmem = TypeTopicCounts::estimate_fit(
                    dumpfile, used_mem, incoming_words);
            num_words += wordsnmem.first;
            used_mem = wordsnmem.second;

            ss.str("");
            if (wordsnmem.first < incoming_words)
                break;
        }
        LOG(WARNING) << num_words << " will fit in " << used_mem
                << " MB of memory";
        LOG(WARNING) << "Initializing Local Dictionary from " << dict_dump
                << " with " << num_words << " words.";
        _dict->initialize_from_dump(dict_dump, num_words, false);
    }
    LOG(WARNING) << "Local Dictionary Initialized. Size: " << _dict->size();
}

WordIndexDictionary& Unigram_Model_Streaming_Builder::get_dict() {
    if (!_dict)
        init_dict();
    return *_dict;
}

Model_Refiner& Unigram_Model_Streaming_Builder::create_model_refiner() {
    Context& context = Context::get_instance();
    context.put_string("output_w", "");
    context.put_string("corpus", "");

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

    _refiner = new Unigram_Model_Streamer(ttc, alpha, beta, get_dict(),
            *_global_dict);
    delete _global_dict;
    return *_refiner;
}

Pipeline& Unigram_Model_Streaming_Builder::create_pipeline(
        Model_Refiner& refiner) {
    _pipeline = new TBB_Pipeline(refiner);
    return *_pipeline;
}

Execution_Strategy& Unigram_Model_Streaming_Builder::create_execution_strategy(
        Pipeline& pipeline) {
    _strategy = new Testing_Execution_Strategy(pipeline, *_model);
    return *_strategy;
}

void Unigram_Model_Streaming_Builder::create_output() {
}

Model& Unigram_Model_Streaming_Builder::get_model() {
    return *_model;
}
