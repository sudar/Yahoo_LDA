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
 * Unigram_Model_Trainer.cpp
 *
 *  Created on: 06-Jan-2011
 *      
 */

#include "Unigram_Model_Trainer.h"
#include "document.pb.h"
#include "sampler.h"
#include "Context.h"
#include "glog/logging.h"
#include "TopicLearner/Dirichlet.h"

Unigram_Model_Trainer::Unigram_Model_Trainer(TypeTopicCounts& ttc,
        Parameter& alpha, Parameter& beta) :
    _ttc(ttc), _alpha(alpha), _beta(beta) {
    Context& context = Context::get_instance();
    ignore_old_topic = context.get_bool("ignore_old_topic");
    string input_w = context.get_string("input_w");
    string input_t = context.get_string("input_t");
    string output_t = context.get_string("output_t");
    set_up_io(input_w, input_t, output_t);

    _num_words = _ttc.get_num_words();
    _num_topics = _ttc.get_num_topics();
    if(ignore_old_topic){
        LOG(WARNING) << "Skipping Word-Topic counts table initialization "
                     << "as online mode has been selected";
    }
    else{
        LOG(WARNING) << "Initializing Word-Topic counts table from docs "
                << input_w << ", " << input_t << " using " << _num_words
                << " words & " << _num_topics << " topics.";
        _ttc.initialize_from_docs(input_w, input_t);
        LOG(WARNING) << "Initialized Word-Topic counts table";
    }

    double alpha_bar = context.get_double("alpha");
    LOG(WARNING) << "Initializing Alpha vector from Alpha_bar = "
            << alpha_bar;
    _alpha.initialize_from_values(_num_topics, NULL, alpha_bar);
    LOG(WARNING) << "Alpha vector initialized";

    double beta_flag = context.get_double("beta");
    LOG(WARNING) << "Initializing Beta Parameter from specified Beta = "
            << beta_flag;
    double beta_value[1] = { beta_flag };
    _beta.initialize_from_values(1, beta_value, _num_words * beta_flag);
    LOG(WARNING) << "Beta param initialized";

    //Initialize the boost RNG
    for (int i = 0; i < NUM_RNGS; i++) {
        generators[i] = new base_generator_type();
        uni_dists[i] = new uniform_real<> (0, 1);
        unif01[i] = new variate_generator<base_generator_type&,
                boost::uniform_real<> > (*(generators[i]), *(uni_dists[i]));
        //Throw away a few initial values
        for (int j = 0; j < 25; j++) {
            (*(unif01[i]))();
        }
    }
    rng_ind = 0;

    //For optimizer
    tau = 100;
    part_grads = new double [_num_topics];
    memset(part_grads,0,_num_topics*sizeof(double));
    part_grads_top_indep = -1 * tau * digamma(alpha.sum);
}

Unigram_Model_Trainer::~Unigram_Model_Trainer() {
    for (int i = 0; i < NUM_RNGS; i++) {
        delete generators[i];
        delete uni_dists[i];
        delete unif01[i];
    }

    release_io();
}

void Unigram_Model_Trainer::set_up_io(string input_w, string input_t,
        string output_t) {
    _wdoc_rdr = new DocumentReader(input_w);

    _tdoc_rdr = (ignore_old_topic) ? NULL : new DocumentReader(input_t);

    _tdoc_writer = new DocumentWriter(output_t);
}

void Unigram_Model_Trainer::release_io() {
    delete _wdoc_rdr;
    if(_tdoc_rdr!=NULL) delete _tdoc_rdr;
    else ignore_old_topic = false;
    delete _tdoc_writer;
}

google::protobuf::Message* Unigram_Model_Trainer::allocate_document_buffer(
        size_t num_docs) {
    return new LDA::unigram_document[num_docs];
}

void Unigram_Model_Trainer::deallocate_document_buffer(
        google::protobuf::Message* docs) {
    delete[] dynamic_cast<LDA::unigram_document*> (docs);
}

google::protobuf::Message* Unigram_Model_Trainer::get_nth_document(
        google::protobuf::Message* docs, size_t n) {
    return dynamic_cast<LDA::unigram_document*> (docs) + n;
}

void* Unigram_Model_Trainer::read(google::protobuf::Message& doc) {
    if (_wdoc_rdr->read(&doc) == -1)
        return NULL;
    LDA::unigram_document* wdoc = dynamic_cast<LDA::unigram_document*> (&doc);
    if(!ignore_old_topic){
        LDA::unigram_document tdoc;
        _tdoc_rdr->read(&tdoc);
        wdoc->MergeFrom(tdoc);
    }
    update_t *upd = new update_t;
    upd->doc = wdoc;
    return upd;
}

void* Unigram_Model_Trainer::sample(void* token) {
    update_t* upd = (update_t*) (token);
    vector<change_elem_t> *updates = 0;
    updates = new vector<change_elem_t> ();
    updates->reserve(EXP_NUM_WORDS_PER_DOC);
    sample_topics(upd, updates);
    return upd;
}

void Unigram_Model_Trainer::sample_topics(update_t* upd,
        std::vector<change_elem_t>* updates) {
    LDA::unigram_document* doc = upd->doc;
    int num_words_in_doc = doc->body_size();

    //Figure out the local topic counts as an indexed
    //dense vector. non_zero_topics is the length of the
    //index array
    topic_t *document_topic_counts = (topic_t*) (calloc(_num_topics,
            sizeof(topic_t)));
    topic_t *document_topic_index = (topic_t*) (calloc(_num_topics,
            sizeof(topic_t)));
    if (!ignore_old_topic)
        for (int k = 0; k < num_words_in_doc; k++) {
            topic_t topic = doc->topic_assignment(k);
            ++document_topic_counts[topic];
        }

    //topic_term_scores & current_topic_counts are allocated here
    //to prevent the allocation & deallocation inside sampler
    double Abar = 0.0;
    double Bbar = 0.0;
    double *C_cac_coeff = new double[_num_topics]; //Store the document static coefficients of C(t,w) here
    double *topic_term_scores = new double[_num_topics]; //C(t)

    topicCounts current_topic_counts(_num_topics); //Storage for topic counts for the current word
    atomic<topic_t> *tokens_per_topic = new atomic<topic_t> [_num_topics]; //Storage for n(t)
    _ttc.get_counts(tokens_per_topic); //Get n(t)

    /***** Compute the index for the local topic counts  *******/
    /***** Along the way compute Abar, Cbar, C_Cac_coeff *******/
    int non_zero_topics = 0;
    atomic<topic_t> *tptInd = tokens_per_topic;
    double *alphaInd = _alpha.values;
    topic_t *ltcInd = document_topic_counts;
    topic_t *ltiInd = document_topic_index;
    double *ccInd = C_cac_coeff;
    register double reg_denom = 0;
    for (topic_t t = 0; t < _num_topics; t++) {
        //Store the most accessed parts of the formula in registers
        reg_denom = *tptInd + _beta.sum;
        register double reg_alphat = *alphaInd;
        register int reg_n = *ltcInd;
        Abar += reg_alphat * _beta.values[0] / reg_denom;
        if (reg_n != 0) {
            *ltiInd = t;
            ++ltiInd;
            ++non_zero_topics;
            Bbar += _beta.values[0] * reg_n / reg_denom;
            *ccInd = (reg_alphat + reg_n) / reg_denom;
        } else {
            *ccInd = reg_alphat / reg_denom;
        }
        ++tptInd;
        ++alphaInd;
        ++ltcInd;
        ++ccInd;
    }
    /***** Compute the index for the local topic counts ******/

    /***** Sample new topics for this document *****/

    do_one_doc(num_words_in_doc, doc, current_topic_counts, tokens_per_topic,
            document_topic_counts, document_topic_index, non_zero_topics, Abar,
            Bbar, C_cac_coeff, topic_term_scores, updates);

    /***** Sample new topics for this document *****/

    //Free resources
    delete[] tokens_per_topic;
    free(document_topic_counts);
    free(document_topic_index);
    delete[] C_cac_coeff;
    delete[] topic_term_scores;

    upd->change_list = updates;
}

void Unigram_Model_Trainer::do_one_doc(int num_words_in_doc,
        LDA::unigram_document *& doc, topicCounts & current_topic_counts,
        atomic<topic_t> *& tokens_per_topic, topic_t *& document_topic_counts,
        topic_t *& document_topic_index, int& non_zero_topics, double &Abar,
        double &Bbar, double *& C_cac_coeff, double *& topic_term_scores,
        vector<change_elem_t> *& updates) {
    //Index into the RNG array
    int rng_index = (num_words_in_doc + non_zero_topics + rng_ind++) & RNG_MASK;

    for (int k = 0; k < num_words_in_doc; k++) {
        //For each word
        change_elem_t elem;
        elem.word = doc->body(k);
        elem.old_topic = (!ignore_old_topic) ? doc->topic_assignment(k) : 0;

        //Store the most accessed parts of the formula for old_topic in registers
        register topic_t reg_old_topic = elem.old_topic;
        register double reg_denom = tokens_per_topic[reg_old_topic] + _beta.sum;
        register double reg_alphaot = _alpha.values[reg_old_topic];
        register int reg_document_topic_countsot =
                document_topic_counts[reg_old_topic];

        //Get the topic counts for current word
        _ttc.get_counts(elem.word, &current_topic_counts);

        if (!ignore_old_topic) {
            /************** Decrement counts of old_topic ********************/
            /************** suitably adjusting masses and cc ********************/
            /************** Make sure registers are up to date *******************/
            //Remove contribution of old_topic to Abar, Bbar
            Abar -= reg_alphaot * _beta.values[0] / reg_denom;
            Bbar -= _beta.values[0] * reg_document_topic_countsot / reg_denom;

            //Decrement n(old_topic,d) and update corresponding register
            --document_topic_counts[reg_old_topic];
            --reg_document_topic_countsot;
            if (reg_document_topic_countsot == 0) {
                //If a count goes to zero update index
                topic_t *zeroedTopic = lower_bound(document_topic_index,
                        document_topic_index + non_zero_topics, reg_old_topic);
                memmove(
                        zeroedTopic,
                        zeroedTopic + 1,
                        (document_topic_index + (non_zero_topics - 1)
                                - zeroedTopic) * sizeof(topic_t));
                --non_zero_topics;
            }

            //Decrement n(t) & update corresponding register
            --tokens_per_topic[reg_old_topic];
            --reg_denom;

            //Add back old_topic contributions after update of counts
            //Recompute Abar, Bbar & C_cac_coeff with the updated counts
            Abar += reg_alphaot * _beta.values[0] / reg_denom;
            Bbar += _beta.values[0] * reg_document_topic_countsot / reg_denom;
            C_cac_coeff[reg_old_topic] = (reg_alphaot
                    + reg_document_topic_countsot) / reg_denom;
            /************** Decrement counts of old_topic ********************/
        }
        /********************* Resample new_topic for the word ****************/
        elem.new_topic = sampler::sample(&current_topic_counts, reg_old_topic,
                tokens_per_topic, document_topic_counts, document_topic_index,
                non_zero_topics, Abar, Bbar, C_cac_coeff, _beta.sum,
                _alpha.values, topic_term_scores, _num_topics,
                unif01[rng_index]);
        LOG_IF(FATAL,elem.new_topic == -1 || elem.new_topic >= _num_topics)<<"Error in sampler. Newtopic generated was: " << elem.new_topic << ". Quitting";
        /********************* Resample new_topic for the word ****************/

        //Store the most accessed parts of the formula for new_topic in registers
        register topic_t reg_new_topic = elem.new_topic;
        register double alphant = _alpha.values[reg_new_topic];
        register int document_topic_countsnt = document_topic_counts[reg_new_topic];
        reg_denom = tokens_per_topic[reg_new_topic] + _beta.sum;

        /************** Increment counts of new_topic ********************/
        /************** suitably adjusting masses and cc ********************/
        /************** Make sure registers are up to date *******************/
        //Remove contribution of new_topic to Abar, Bbar
        Abar -= alphant * _beta.values[0] / reg_denom;
        Bbar -= _beta.values[0] * document_topic_countsnt / reg_denom;

        //Increment n(new_topic,d) and update corresponding register
        ++document_topic_counts[reg_new_topic];
        ++document_topic_countsnt;
        if(document_topic_countsnt == 1) {
            //If a new count gets added update the index
            topic_t *newTopPos = lower_bound(document_topic_index, document_topic_index + non_zero_topics, reg_new_topic);
            memmove(newTopPos + 1, newTopPos, (document_topic_index + non_zero_topics - newTopPos) * sizeof (topic_t));
            *newTopPos = reg_new_topic;
            ++non_zero_topics;
        }

        //Increment n(t) & update corresponding register
        ++tokens_per_topic[reg_new_topic];
        ++reg_denom;

        //Add back new_topic contributions after update of counts
        //Recompute Abar, Bbar & C_cac_coeff with the updated counts
        Abar += alphant * _beta.values[0] / reg_denom;
        Bbar += _beta.values[0] * document_topic_countsnt / reg_denom;
        C_cac_coeff[reg_new_topic] = (alphant + document_topic_countsnt) / reg_denom;
        /************** Increment counts of new_topic ********************/

        //If there is no change in topic assignment
        //Dont send it to the updater
        //if(elem.new_topic == elem.old_topic)
        //continue;

        //Note the new topic assignment in doc
        if(!ignore_old_topic)
        doc->set_topic_assignment(k, elem.new_topic);
        else
        doc->add_topic_assignment(elem.new_topic);

        //Push the update msg if not in testing mode
        if(updates!=NULL)
        updates->push_back(elem);
    }
}

void* Unigram_Model_Trainer::update(void* token) {
    update_t* upd = (update_t*) token;
    size_t sz = upd->change_list->size();
    for (size_t i = 0; i < sz; i++) {
        change_elem_t change = (*upd->change_list)[i];
        //		cout << "(" << dict->get_word(change.word) << "," << change.old_topic << "-" << change.new_topic << ")" << endl;
        //		cout << _ttc.print(change.word) << endl;
        _ttc.upd_count(change.word, change.old_topic, change.new_topic,
                ignore_old_topic);
        //		cout << _ttc.print(change.word) << endl;
    }
    if (sz != 0) {
        upd->change_list->clear();
    }
    if (upd->change_list != NULL)
        delete upd->change_list;

    return upd;
}

void* Unigram_Model_Trainer::optimize(void* token) {
    update_t* upd = (update_t*) token;
    LDA::unigram_document& doc = *(upd->doc);
    topic_t* local_topic_counts = new topic_t[_num_topics];
    topic_t* local_topic_index = new topic_t[_num_topics];
    int num_words_in_doc = doc.body_size();

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

    /************* Optimize **************/
    //		cout << "Optimizing: " << doc_index << endl;
    ++doc_index;
    if (doc_index != 0 && doc_index % tau == 0) {
        //Update global alphas with the partial
        //gradients that have been computed till now
        double* part_grads_index = part_grads;
        double *global_alphas_index = _alpha.values;
        double sum = 0.0;
        eta = pow((doc_index / tau) + 100, -0.5) * (0.1 / tau);
        for (topic_t t = 0; t < _num_topics; t++) {
            *global_alphas_index = max(
                    *global_alphas_index - eta * (*part_grads_index
                            + part_grads_top_indep), MIN_ALPHA);
            sum += *global_alphas_index;
            ++global_alphas_index;
            ++part_grads_index;
        }
        _alpha.sum = sum;

        //Reset the partial gradients
        memset(part_grads, 0, _num_topics * sizeof(double));
        part_grads_top_indep = -1 * tau * digamma(_alpha.sum);
    }
    //Update topic dependent & topic independent partial gradients
    //with contributions from this document
    ltdInd = local_topic_index;
    for (int i = 0; i < non_zero_topics; i++) {
        topic_t topic = *ltdInd;
        part_grads[topic] += digamma(_alpha.values[topic]) - digamma(
                _alpha.values[topic] + local_topic_counts[topic]);
        ++ltdInd;
    }
    part_grads_top_indep += digamma(_alpha.sum + num_words_in_doc);

    /************* Optimize **************/
    delete[] local_topic_counts;
    delete[] local_topic_index;

    return upd;
}

void* Unigram_Model_Trainer::eval(void* token, double& eval_value) {
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

void Unigram_Model_Trainer::write(void* token) {
    update_t* upd = (update_t*) token;
    LDA::unigram_document& doc = *(upd->doc);
    doc.clear_body();
    LOG_IF(FATAL, !_tdoc_writer->write(doc) )<< "Couldn't write to the stream. Quitting..." << endl;
    delete upd;
}

void Unigram_Model_Trainer::iteration_done() {
    Context& context = Context::get_instance();
    string input_w = context.get_string("input_w");
    string input_t = context.get_string("input_t");
    string output_t = context.get_string("output_t");

    string cmd = "mv " + output_t + " " + input_t;
    system(cmd.c_str());
    LOG(WARNING) << "Restarting IO";
    release_io();
    set_up_io(input_w, input_t, output_t);
}

void* Unigram_Model_Trainer::test(void* token) {
    LOG(FATAL) << "Test method called for trainer";
}

long Unigram_Model_Trainer::doc_index = -1;
