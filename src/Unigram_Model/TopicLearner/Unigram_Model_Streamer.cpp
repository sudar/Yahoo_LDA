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
 * Unigram_Model_Streamer.cpp
 *
 *  Created on: 28-Mar-2011
 *      
 */

#include "Unigram_Model_Streamer.h"
#include "Context.h"
#include "glog/logging.h"
#include "document.pb.h"

Unigram_Model_Streamer::Unigram_Model_Streamer(TypeTopicCounts& ttc,
        Parameter& alpha, Parameter& beta, WordIndexDictionary& local_dict,
        WordIndexDictionary& global_dict) :
    Unigram_Model_Tester(ttc, alpha, beta, local_dict, true),
            _local_dict(local_dict) {
    _wdoc_rdr = NULL;
    _tdoc_writer = NULL;
    Context& context = Context::get_instance();
    _num_words = _ttc.get_num_words();
    _num_topics = _ttc.get_num_topics();
    string ttc_dumpfile = context.get_string("ttc_dumpfile");
    string param_dumpfile = context.get_string("param_dumpfile");

    int num_dumps = context.get_int("numdumps");
    if (num_dumps == 1) {
        LOG(WARNING) << "Initializing Word-Topic counts table from dump "
                << ttc_dumpfile << " using " << _num_words << " words & "
                << _num_topics << " topics.";
        _ttc.initialize_from_dump(ttc_dumpfile, &_local_dict, &global_dict);
    } else {
        stringstream ss;
        int tot_num_words = global_dict.get_num_words();
        int num_words_per_client = tot_num_words / num_dumps;
        LOG(WARNING) << "Initializing Word-Topic counts table from "
                << num_dumps << " dumps with " << ttc_dumpfile
                << " as prefix using " << _num_words << " words & "
                << _num_topics << " topics.";
        for (int dump = 0; dump < num_dumps; dump++) {
            ss << ttc_dumpfile;
            ss << "." << dump;
            string dumpfile = ss.str();
            int offset = dump * num_words_per_client;
            bool cont_init = _ttc.initialize_from_dump(dumpfile, &_local_dict,
                    &global_dict, offset);
            if (!cont_init)
                break;
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

Unigram_Model_Streamer::~Unigram_Model_Streamer() {
    // TODO Auto-generated destructor stub
}

int Unigram_Model_Streamer::insert_word_to_dict(std::string word) {
    return _local_dict.get_index(word);
}

void* Unigram_Model_Streamer::read(google::protobuf::Message& doc) {
    LDA::unigram_document* uni_doc =
            dynamic_cast<LDA::unigram_document*> (&doc);
    LDA::unigram_document& wdoc = *uni_doc;
    int ret = read_from_inp(wdoc, std::cin);
    if (ret == -1)
        return NULL;
    topic_t top;
    for (int i = 0; i < wdoc.body_size(); i++) {
        top = rand() % _num_topics;
        wdoc.add_topic_assignment(top);
    }
    update_t *upd = new update_t;
    upd->doc = uni_doc;
    return upd;
}

void Unigram_Model_Streamer::write(void* token) {
    update_t* upd = (update_t*) token;
    LDA::unigram_document& doc = *(upd->doc);
    int num_words_in_doc = doc.body_size();
    //Write word to topic assignments
    ostream& txt_output = std::cout;
    txt_output << doc.docid() << "\t" << doc.url() << "\t";
    for (int i = 0; i < num_words_in_doc; i++) {
        txt_output << "(" << _local_dict.get_word(doc.body(i)) << ","
                << doc.topic_assignment(i) << ")";
        if (i < num_words_in_doc - 1)
            txt_output << " ";
    }
    txt_output << endl;
    delete upd;
}

void Unigram_Model_Streamer::iteration_done() {
}
