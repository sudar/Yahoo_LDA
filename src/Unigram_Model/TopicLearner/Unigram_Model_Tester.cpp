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
 * Unigram_Model_Tester.cpp
 *
 *  Created on: 06-Jan-2011
 *      
 */

#include "Unigram_Model_Tester.h"
#include "document.pb.h"
#include "sampler.h"
#include "Context.h"
#include "glog/logging.h"
#include "TopicLearner/Dirichlet.h"

Unigram_Model_Tester::Unigram_Model_Tester(TypeTopicCounts& ttc,
        Parameter& alpha, Parameter& beta, WordIndexDictionary& local_dict,
        bool no_init) :
    _ttc(ttc), _alpha(alpha), _beta(beta) {
    if (no_init)
        return;
    Context& context = Context::get_instance();
    string input_w = context.get_string("input_w");
    string output_t = context.get_string("output_t");
    set_up_io(input_w, output_t);

    _num_words = _ttc.get_num_words();
    _num_topics = _ttc.get_num_topics();

    string ttc_dumpfile = context.get_string("ttc_dumpfile");
    string param_dumpfile = context.get_string("param_dumpfile");
    string global_dict_dump = context.get_string("global_dict_dump");
    WordIndexDictionary global_dict;
    global_dict.initialize_from_dump(global_dict_dump);

    int num_dumps = context.get_int("numdumps");
    if (num_dumps == 1) {
        LOG(WARNING) << "Initializing Word-Topic counts table from dump "
                << ttc_dumpfile << " using " << _num_words << " words & "
                << _num_topics << " topics.";
        _ttc.initialize_from_dump(ttc_dumpfile, &local_dict, &global_dict);
    } else {
        LOG(WARNING) << "Initializing Word-Topic counts table from "
                << num_dumps << " dumps with " << ttc_dumpfile
                << " as prefix using " << _num_words << " words & "
                << _num_topics << " topics.";
        int num_words = global_dict.get_num_words();
        int num_words_per_client = num_words / num_dumps;
        stringstream ss;
        for (int dump = 0; dump < num_dumps; dump++) {
            ss << ttc_dumpfile;
            ss << "." << dump;
            string dumpfile = ss.str();
            int offset = dump * num_words_per_client;
            _ttc.initialize_from_dump(dumpfile, &local_dict, &global_dict,
                    offset);
            ss.str("");
        }

    }
    LOG(WARNING) << "Initialized Word-Topic counts table";

    double alpha_bar = context.get_double("alpha");
    LOG(WARNING) << "Initializing Alpha vector from dumpfile "
            << param_dumpfile;
    _alpha.initialize_from_dump(param_dumpfile);
    LOG(WARNING) << "Alpha vector initialized";

    double beta_flag = context.get_double("beta");
    LOG(WARNING) << "Initializing Beta Parameter from specified Beta = "
            << beta_flag;
    double beta_value[1] = { beta_flag };
    _beta.initialize_from_values(1, beta_value, _num_words * beta_flag);
    LOG(WARNING) << "Beta param initialized";

    ignore_old_topic = false;
}

Unigram_Model_Tester::~Unigram_Model_Tester() {
    release_io();
}

void Unigram_Model_Tester::set_up_io(string input_w, string output_t) {
    _wdoc_rdr = new DocumentReader(input_w);

    _tdoc_writer = new DocumentWriter(output_t);
}

void Unigram_Model_Tester::release_io() {
    if (_wdoc_rdr)
        delete _wdoc_rdr;
    if (_tdoc_writer)
        delete _tdoc_writer;
}

google::protobuf::Message* Unigram_Model_Tester::allocate_document_buffer(
        size_t num_docs) {
    return new LDA::unigram_document[num_docs];
}

void Unigram_Model_Tester::deallocate_document_buffer(
        google::protobuf::Message* docs) {
    delete[] dynamic_cast<LDA::unigram_document*> (docs);
}

google::protobuf::Message* Unigram_Model_Tester::get_nth_document(
        google::protobuf::Message* docs, size_t n) {
    return dynamic_cast<LDA::unigram_document*> (docs) + n;
}

void* Unigram_Model_Tester::read(google::protobuf::Message& doc) {
    if (_wdoc_rdr->read(&doc) == -1)
        return NULL;
    LDA::unigram_document* wdoc = dynamic_cast<LDA::unigram_document*> (&doc);
    update_t *upd = new update_t;
    upd->doc = wdoc;
    return upd;
}

void* Unigram_Model_Tester::test(void* token) {
    update_t* upd = (update_t*) token;
    LDA::unigram_document& doc = *(upd->doc);
    int num_words_in_doc = doc.body_size();
    //Populate the set of unique words
    unordered_map<int, int> seen_words;
    size_t num_uniq_words = 0;
    for (int k = 0; k < num_words_in_doc; k++) {
        int word = doc.body(k);
        if (seen_words.count(word) != 0)
            continue;
        pair<int, int> p(word, ++num_uniq_words);
        seen_words.insert(p);
    }

    //Create the P(Wi|j) matrix
    //Allocate enough space
    LOG_IF(FATAL,num_uniq_words!=seen_words.size())<<"Something is wrong in finding unique words. map size: " << seen_words.size() << " counter: " << num_uniq_words;
    int* word_frequency = new int [num_uniq_words];
    memset(word_frequency,0,num_uniq_words*sizeof(int));
    for(int k = 0;k < num_words_in_doc;k++) {
        int word = doc.body(k);
        int uniq_word_ind = seen_words[word] - 1;
        ++word_frequency[uniq_word_ind];
    }

    float** wij_prob = new float* [num_uniq_words];
    for(size_t i=0;i<num_uniq_words;i++) {
        wij_prob[i] = new float [_num_topics];
    }

    //Fill in the prob values
    unordered_map<int,int>::iterator end = seen_words.end();
    topicCounts current_topic_counts(_num_topics);
    atomic<topic_t> *tokens_per_topic = new atomic<topic_t> [_num_topics]; //Storage for n(t)
    _ttc.get_counts(tokens_per_topic); //Get n(t)
    for(unordered_map<int,int>::iterator iter=seen_words.begin(); iter!=end; iter++) {
        //Get the topic counts for current word
        word_t word = (*iter).first;
        _ttc.get_counts(word, &current_topic_counts);
        int word_ind = (*iter).second - 1;
        float* row = wij_prob[word_ind];
        for(int j=0;j<_num_topics;j++) {
            row[j] = _beta.values[0] / (float)(tokens_per_topic[j] + _beta.sum);
        }
        size_t length = current_topic_counts.length;
        for (size_t j = 0; j < length; j++) {
            cnt_topic_t& ct = current_topic_counts.items[j];
            topic_t top = ct.choose.top;
            row[top] = (ct.choose.cnt + _beta.values[0]) / (float)(tokens_per_topic[top] + _beta.sum);
            //cout << top << ", " << row[top] << "  ";
        }
    }

    //Initialize P(j|d) with alphas
    float* topic_prob = new float [_num_topics];
    for(int i=0;i<_num_topics;i++) {
        topic_prob[i] = _alpha.values[i] / _alpha.sum;
    }

    float* Denom = new float [num_uniq_words];
    float* cur_top_prob = new float [_num_topics];
    int iters = 15;
    do {
        //Compute the denominator
        for(size_t i=0;i<num_uniq_words;i++) {
            float sum = 0.;
            float* row = wij_prob[i];
            for(int j=0;j<_num_topics;j++)
            sum += row[j] * topic_prob[j];

            Denom[i] = sum;
        }
        //Compute current P(j|d)
        for(int j=0;j<_num_topics;j++)
        cur_top_prob[j] = _alpha.values[j];// - 1;

        for(size_t i=0;i<num_uniq_words;i++) {
            float* row = wij_prob[i];
            float denom = Denom[i];
            int word_freq = word_frequency[i];
            float sum = 0.;
            for(int j=0;j<_num_topics;j++) {
                cur_top_prob[j] += word_freq * row[j] * topic_prob[j] / denom;
                sum += row[j] * topic_prob[j] / denom;
            }
        }

        //replace prev topic_prob with cur_top_prob
        float const_den = num_words_in_doc + _alpha.sum;// - _num_topics;
        float prob_sum = 0.;
        for(int j=0;j<_num_topics;j++) {
            float tmp_ctp = cur_top_prob[j]/const_den;
            topic_prob[j] = tmp_ctp;
            prob_sum += tmp_ctp;
        }
    }while(--iters>0);

    //Set the topic distribution for this document
    for(int j=0;j<_num_topics;j++)
    doc.add_topic_distribution(topic_prob[j]);
    //Compute the topic assignments per word
    end = seen_words.end();
    for(unordered_map<int,int>::iterator iter=seen_words.begin(); iter!=end; iter++) {
        word_t word = (*iter).first;
        int word_ind = (*iter).second - 1;
        float* row = wij_prob[word_ind];
        float denom = Denom[word_ind];
        float max = 0.;
        float sum = 0.;
        int& max_top = seen_words[word];

        for(int j=0;j<_num_topics;j++) {
            float prob = row[j] * topic_prob[j] / denom;
            sum += prob;
            if(prob > max) {
                max = prob;
                max_top = j;
            }
        }
    }
    for(int k = 0;k < num_words_in_doc;k++) {
        word_t word = doc.body(k);
        doc.add_topic_assignment(seen_words[word]);
    }

    delete [] word_frequency;
    delete [] cur_top_prob;
    delete [] Denom;
    delete [] topic_prob;
    delete [] tokens_per_topic;
    for(size_t i=0;i<num_uniq_words;i++)
    delete [] wij_prob[i];
    delete [] wij_prob;

    return upd;
}

void* Unigram_Model_Tester::sample(void*) {
    LOG(FATAL) << "Sample method called for tester";
}

void* Unigram_Model_Tester::update(void*) {
    LOG(FATAL) << "Update method called for tester";
}

void* Unigram_Model_Tester::optimize(void*) {
    LOG(FATAL) << "Optimize method called for tester";
}

void* Unigram_Model_Tester::eval(void* token, double& eval_value) {
    update_t* upd = (update_t*) token;
    LDA::unigram_document& doc = *(upd->doc);

    double doc_loglikelihood = 0.0;

    topic_t* local_topic_counts = new topic_t[_num_topics];
    topic_t* local_topic_index = new topic_t[_num_topics];
    int num_words_in_doc = doc.body_size();

    /************* Compute Likelihood **************/
    //Clear topic Counts
    memset(local_topic_counts, 0, _num_topics * sizeof(topic_t));
    memset(local_topic_index, 0, _num_topics * sizeof(topic_t));

    int non_zero_topics = 0;
    topic_t* ltcInd = local_topic_counts;
    topic_t* ltdInd = local_topic_index;
    if (!ignore_old_topic) {
        //Build the dense local_topic_counts with index
        for (int k = 0; k < num_words_in_doc; k++) {
            topic_t topic = doc.topic_assignment(k);
            ++local_topic_counts[topic];
        }

        for (topic_t t = 0; t < _num_topics; t++) {
            int n = *ltcInd;
            if (n != 0) {
                *ltdInd = t;
                ++ltdInd;
                ++non_zero_topics;
            }
            ++ltcInd;
        }
        //----- Local Topic Counts built
    }

    //Accumulate loglikelihood for this doc
    ltdInd = local_topic_index;
    for (int i = 0; i < non_zero_topics; i++) {
        topic_t topic = *ltdInd;
        double gal = _alpha.values[topic];
        int cnt = local_topic_counts[topic];
        doc_loglikelihood += log_gamma(gal + cnt) - log_gamma(gal);

        LOG_IF(FATAL,isnan(doc_loglikelihood))<< gal << "," << cnt << "," <<_alpha.sum << "," << num_words_in_doc;

        ++ltdInd;
    }
    doc_loglikelihood += log_gamma(_alpha.sum) - log_gamma(_alpha.sum + num_words_in_doc);
    /************* Compute Likelihood **************/

    delete [] local_topic_counts;
    delete [] local_topic_index;

    eval_value = doc_loglikelihood;
    return upd;
}

void Unigram_Model_Tester::write(void* token) {
    update_t* upd = (update_t*) token;
    LDA::unigram_document& doc = *(upd->doc);
    doc.clear_body();
    LOG_IF(FATAL, !_tdoc_writer->write(doc) )<< "Couldn't write to the stream. Quitting..." << endl;
    delete upd;
}

void Unigram_Model_Tester::iteration_done() {
    release_io();
    Context& context = Context::get_instance();
    string input_w = context.get_string("input_w");
    string input_t = context.get_string("input_t");
    string output_t = context.get_string("output_t");

    string cmd = "cp " + output_t + " " + input_t;
    system(cmd.c_str());
}
