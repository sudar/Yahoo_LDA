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
 * Unigram_Model_Builder.cpp
 *
 *  Created on: 06-Jan-2011
 *      
 */

#include "Unigram_Model_Training_Builder.h"
#include "Context.h"
#include "DocumentReader.h"
#include "DocumentWriter.h"
#include "Unigram_Model_Trainer.h"
#include "TopicLearner/TBB_Pipeline.h"
#include "TopicLearner/Training_Execution_Strategy.h"
#include "Local_Checkpointer.h"

Unigram_Model_Training_Builder::Unigram_Model_Training_Builder() {
    _dict = NULL;
    Context& context = Context::get_instance();
    string input_prefix = context.get_string("inputprefix");
    string dict_dump = input_prefix + ".dict" + ".dump";
    context.put_string("dictdump", dict_dump);
    WordIndexDictionary& dict = get_dict();

    int num_words = dict.get_num_words();
    int num_topics = context.get_int("topics");

    _model = new Unigram_Model(num_words, num_topics);
}

Unigram_Model_Training_Builder::~Unigram_Model_Training_Builder() {
    delete _dict;
    delete _refiner;
    delete _pipeline;
    delete _strategy;
    delete _checkpointer;
    delete _model;
}

void Unigram_Model_Training_Builder::init_dict() {
    _dict = new WordIndexDictionary;
    string dict_dump = Context::get_instance().get_string("dictdump");
    LOG(WARNING) << "Initializing Dictionary from " << dict_dump;
    _dict->initialize_from_dump(dict_dump);
    LOG(WARNING) << "Dictionary Initialized";
}

WordIndexDictionary& Unigram_Model_Training_Builder::get_dict() {
    if (!_dict)
        init_dict();
    return *_dict;
}

void Unigram_Model_Training_Builder::initialize_topics(string input_w,
        string input_t, int num_topics) {
    std::ifstream tp_out(input_t.c_str());
    if (tp_out.is_open()) {
        LOG(WARNING)
                << "Topics file already exists. Skipping random topic initialization";
        return;
    }
    bool online_mode = Context::get_instance().get_bool("online");
    if(online_mode) {
        Context::get_instance().put_string("ignore_old_topic","true");
        return;
    }
    DocumentReader rdr(input_w);
    DocumentWriter wrt(input_t);
    LDA::unigram_document wdoc, tdoc;
    while (rdr.read(&wdoc) != -1) {
        topic_t top;
        tdoc.Clear();
        for (int i = 0; i < wdoc.body_size(); i++) {
            top = rand() % num_topics;
            tdoc.add_topic_assignment(top);
        }
        wrt.write(tdoc);
    }
}

Model_Refiner& Unigram_Model_Training_Builder::create_model_refiner() {
    Context& context = Context::get_instance();
    string input_prefix = context.get_string("inputprefix");
    string input_w = input_prefix + ".wor";
    string input_t = input_prefix + ".top";
    string output_t = input_prefix + ".top.tmp";
    int num_topics = context.get_int("topics");
    context.put_string("ignore_old_topic","false");
    initialize_topics(input_w, input_t, num_topics);
    context.put_string("input_w", input_w);
    context.put_string("input_t", input_t);
    context.put_string("output_t", output_t);

    TypeTopicCounts& ttc = _model->get_ttc();
    Parameter& alpha = _model->get_parameter(Unigram_Model::ALPHA);
    Parameter& beta = _model->get_parameter(Unigram_Model::BETA);

    _refiner = new Unigram_Model_Trainer(ttc, alpha, beta);
    return *_refiner;
}

Pipeline& Unigram_Model_Training_Builder::create_pipeline(
        Model_Refiner& refiner) {
    _pipeline = new TBB_Pipeline(refiner);
    return *_pipeline;
}

Execution_Strategy& Unigram_Model_Training_Builder::create_execution_strategy(
        Pipeline& pipeline) {
    _checkpointer = new Local_Checkpointer();
    _strategy = new Training_Execution_Strategy(pipeline, *_model,
            *_checkpointer);
    return *_strategy;
}

void Unigram_Model_Training_Builder::create_output() {
    Context& context = Context::get_instance();
    string output_t = context.get_string("output_t");
    string cmd = "rm " + output_t;
    system(cmd.c_str());

    string input_prefix = context.get_string("inputprefix");
    string ttcdump = input_prefix + ".ttc" + ".dump";
    string paramdump = input_prefix + ".par" + ".dump";
    context.put_string("ttcdump", ttcdump);
    context.put_string("paramdump", paramdump);
    _model->save();
    string tw_out = input_prefix + ".topToWor" + ".txt";
    context.put_string("tw_out", tw_out);
    _model->write_statistics(get_dict());

    string tp_out = input_prefix + ".docToTop" + ".txt";
    std::ofstream tp_output(tp_out.c_str());
    LOG_IF(FATAL,!tp_output.is_open())<< "Unable to open output file: " << tp_out;
    string txt_out = input_prefix + ".worToTop" + ".txt";
    std::ofstream txt_output(txt_out.c_str());
    LOG_IF(FATAL,!txt_output.is_open()) << "Unable to open output file: " << txt_out;
    LOG(WARNING) << "Saving document to topic-proportions in " << tp_out;
    LOG(WARNING) << "Saving word to topic assignment in " << txt_out;

    string input_w = context.get_string("input_w");
    string input_t = context.get_string("input_t");
    int num_topics = context.get_int("topics");
    DocumentReader wdoc_rdr(input_w), tdoc_rdr(input_t);
    LDA::unigram_document doc;
    while(wdoc_rdr.read(&doc)!=-1) {
        LDA::unigram_document tdoc;
        tdoc_rdr.read(&tdoc);
        doc.MergeFrom(tdoc);
        //For each document
        int num_words_in_doc = doc.body_size();
        {
            //Write Topic Proportions
            vector<tppair> topProb;
            //Write first entry
            tp_output << doc.docid() << "\t" << doc.url() << "\t";

            topic_t* local_topic_counts = (topic_t*)calloc(num_topics,sizeof(topic_t));

            for(int k=0;k<num_words_in_doc;k++) {
                topic_t topic = doc.topic_assignment(k);
                ++local_topic_counts[topic];
            }

            //Store the topic, counts as proportion in a vector
            topic_t* ltcInd = local_topic_counts;
            for(topic_t t=0;t<num_topics;t++) {
                int n = *ltcInd;
                if(n!=0) {
                    tppair tp(t,(float)n/num_words_in_doc);
                    topProb.push_back(tp);
                }
                ++ltcInd;
            }
            free(local_topic_counts);

            //Sort on the counts proportion
            std::sort(topProb.begin(),topProb.end(),prob_cmp);

            //Write out the second entry
            for(int i=0;i<min((int)topProb.size(),NUM_TOPICS_PER_DOC);i++) {
                tp_output << "(" << topProb[i].first << "," << topProb[i].second << ")";
                if(i<min((int)topProb.size(),NUM_TOPICS_PER_DOC)-1)
                tp_output << " ";
            }
            tp_output << endl;
        }
        {
            //Write word to topic assignments
            txt_output << doc.docid() << "\t" << doc.url() << "\t";
            for(int i=0;i<num_words_in_doc;i++) {
                txt_output << "(" << _dict->get_word(doc.body(i)) << "," << doc.topic_assignment(i) << ")";
                if(i<num_words_in_doc-1)
                txt_output << " ";
            }
            txt_output << endl;

        }
    }
    tp_output.flush();
    tp_output.close();
    txt_output.flush();
    txt_output.close();

    LOG(WARNING) << "Document to topic-proportions saved in " << tp_out;
    LOG(WARNING) << "Word to topic assignment saved in " << txt_out;
}

Model& Unigram_Model_Training_Builder::get_model() {
    return *_model;
}
