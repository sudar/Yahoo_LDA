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
 * Unigram_Model_Trainer.h
 *
 *  Created on: 06-Jan-2011
 *      
 */

#ifndef UNIGRAM_MODEL_TRAINER_H_
#define UNIGRAM_MODEL_TRAINER_H_

#include "TopicLearner/Model_Refiner.h"
#include "TypeTopicCounts.h"
#include "TopicLearner/Parameter.h"
#include "DocumentReader.h"
#include "DocumentWriter.h"
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>

using namespace boost;
using namespace std;

/**
 * The default implementation of Model_Refiner for the
 * Unigram model
 */
class Unigram_Model_Trainer: public Model_Refiner {
public:
    Unigram_Model_Trainer(TypeTopicCounts&, Parameter&, Parameter&);
    virtual ~Unigram_Model_Trainer();

    google::protobuf::Message* allocate_document_buffer(size_t);
    void deallocate_document_buffer(google::protobuf::Message*);
    google::protobuf::Message* get_nth_document(
            google::protobuf::Message* docs, size_t n);

    //!Reads a document from the protobuf
    //!format word & topic files using DocumentReader
    void* read(google::protobuf::Message&);

    //!Does Gibbs sampling using sampler.cpp to
    //!figure out new topic assignments to each
    //!word present in the document passed in the msg
    void* sample(void*);

    //!Takes a msg which contains the document to be
    //!processed and the updated topics for each word
    //!in the document as a vector. It then processes
    //!each update by just calling upd_count on the
    //!TypeTopicCounts object with the update details
    void* update(void*);

    //!Performs stochastic GD to optimize the alphas. The
    //!gradients are accumulated for tau docs and then the
    //!global alphas are updated.
    void* optimize(void*);

    //!Compute the document portion of the log-likelihood
    void* eval(void*, double&);

    //!Takes the document and writes it to disk. Here we
    //!use a simple optimization of not writing the
    //!body/words in the document but only the topics.
    //!This is because the words in the document never
    //!change. Its only the topics that change. The documents
    //!are written using a DocumentWriter to disk
    void write(void*);

    void iteration_done();

    void* test(void*);

    static long doc_index; //!Running count of all the documents processed by the optimizer

private:
    void set_up_io(string, string, string);
    void release_io();
    //!Sampler
    void do_one_doc(int num_words_in_doc, LDA::unigram_document *& doc,
            topicCounts & current_topic_counts,
            atomic<topic_t> *& tokens_per_topic,
            topic_t *& document_topic_counts, topic_t *& document_topic_index,
            int& non_zero_topics, double &Abar, double &Bbar,
            double *& C_cac_coeff, double *& topic_term_scores,
            vector<change_elem_t> *& updates);

    void sample_topics(update_t* upd, vector<change_elem_t> *updates);

private:
    TypeTopicCounts& _ttc;
    Parameter& _alpha;
    Parameter& _beta;
    bool ignore_old_topic;
    int _num_words, _num_topics;
    //!Reader
    DocumentReader *_wdoc_rdr, *_tdoc_rdr;
    DocumentWriter *_tdoc_writer;

    //!Sampler
    //!The structures needed to setup boost RNG
    //!Its a combination of a variate generator
    //!and distribution object. We create an
    //!array of RNGs since its hard to maintain
    //!thread local state in TBB. We choose an
    //!index into the array randomly and hope
    //!different threads land up at different
    //!RNGS
    base_generator_type *generators[NUM_RNGS];
    uniform_real<> *uni_dists[NUM_RNGS];
    variate_generator<base_generator_type&, boost::uniform_real<> >
            *unif01[NUM_RNGS];
    long rng_ind;

    //!Optimizer
    double *part_grads, //!Local Alphas into which we accumulate the gradients
            part_grads_top_indep; //!Local AlphaBar
    int tau; //!The number documents to accumulate gradients
    double eta; //!The fraction of the gradient to be merged into global alphas
};

#endif /* UNIGRAM_MODEL_TRAINER_H_ */
