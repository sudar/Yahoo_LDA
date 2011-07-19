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
 * sampler.cpp
 *
 *  Created on: 24 Apr, 2009
 *      
 */
#include <algorithm>
#include <iostream>
#include "sampler.h"
#include "Context.h"

using namespace std;
namespace sampler {

topic_t sample(const topicCounts* currentTypeTopicCounts, topic_t old_topic,
        const atomic<topic_t>* tokens_per_topic,
        const topic_t* local_topic_counts, const topic_t* local_topic_index,
        const int& non_zero_topics, const double& smoothingOnlyMass,
        const double& topicBetaMass, const double* cachedCoefficients,
        const double& betaSum, const double* alpha, double* topic_term_scores,
        const topic_t& numTopics,
        variate_generator<base_generator_type&, boost::uniform_real<> >* unif01) {

    float beta = Context::get_instance().get_double("beta");
    //Cbar will be stored here
    register double topicTermMass = 0.0;

    // Go over the type/topic counts calculating the score
    // for each topic. This is basically calculation of C(t,w) where
    // w is the word whose topic is being sampled
    // We also use index pointers for both topics & counts
    // separately to avoid the use of the packed structure
    // directly and extra memory accesses. But in the process
    // we loose portability.
    int length = currentTypeTopicCounts->length;
    cnt_topic_t* items = currentTypeTopicCounts->items;
    register topic_t* topics = (topic_t*) items;
    cnt_t* cnts = (cnt_t*) (items) + 1;
    register topic_t *endTop = (topic_t*) (items + length - 1);

    //C(t,w) will be stored into topic_term_scores
    register double* ttsptr = topic_term_scores;
    for (; topics <= endTop; topics += 2) {
        register topic_t currentTopic = *topics;
        *ttsptr
                = (currentTopic == old_topic) ? cachedCoefficients[currentTopic]
                        * (*cnts - 1)
                        : cachedCoefficients[currentTopic] * *cnts;
        topicTermMass += *ttsptr;
        ++ttsptr;
        cnts += 2;
    }

    double massSum = smoothingOnlyMass + topicBetaMass + topicTermMass;
    double sample = (*unif01)() * (massSum);
    double origSample = sample;
    //	Make sure it actually gets set
    topic_t new_topic = -1;

    // Check if sample < Cbar
    if (sample < topicTermMass) {
        //Handle overflow by preassigning last topic from the topics for this word
        //that can contribute to the mass.
        //TODO: instead of endTop the index bound can be endTop-1
        new_topic = *endTop;
        ttsptr = topic_term_scores;
        topics = (topic_t*) items;
        for (; topics < endTop; topics += 2) {
            sample -= *(ttsptr++);
            if (sample <= 0.0) {
                //Found our topic sample
                new_topic = *topics;
                break;
            }
        }
    } else {
        // sample > Cbar
        // sample -= Cbar
        sample -= topicTermMass;

        //Check if sample < Bbar
        if (sample < topicBetaMass) {
            //predivide by Beta to save computation
            sample /= beta;
            //Handle overflow by preassigning the last topic with non zero count
            //in the document-topic counts for this document
            //TODO: instead of non_zero_topics bounds can be non_zero_topics-1
            new_topic = local_topic_index[non_zero_topics - 1];
            for (topic_t denseIndex = 0; denseIndex < non_zero_topics - 1; denseIndex++) {
                topic_t topic = local_topic_index[denseIndex];
                sample -= local_topic_counts[topic] / (tokens_per_topic[topic]
                        + betaSum);

                if (sample <= 0.0) {
                    //Found our topic sample
                    new_topic = topic;
                    break;
                }
            }

        } else {
            //Sample > Cbar + Bbar
            // sample -= Bbar as we have already taken out Cbar
            sample -= topicBetaMass;

            //predivide by Beta to save computation
            sample /= beta;

            //Handle overflow by preassigning the last topic
            //TODO: instead of num_topics, bounds can be num_topics-1
            new_topic = numTopics - 1;
            for (int i = 0; i < numTopics - 1; i++) {
                sample -= alpha[i] / (tokens_per_topic[i] + betaSum);
                if (sample <= 0.0) {
                    //Found our topic sample
                    new_topic = i;
                    break;
                }
            }
        }
    }

    // Found an invalid topic. Dump the various values
    // to find the reason
    if (new_topic == -1 || new_topic >= numTopics) {
        stringstream ss;
        ss << "Sample: " << origSample << endl;
        ss << "Sample left: " << sample << endl;
        ss << "Num Topics: " << numTopics << endl;
        ss << "Non Zero Topics: " << non_zero_topics << endl;
        ss << "Old Topic: " << old_topic << endl;
        ss << "New Topic: " << new_topic << endl;
        ss << "Smoothing Mass: " << smoothingOnlyMass << endl;
        ss << "Topic Beta Mass: " << topicBetaMass << endl;
        ss << "Topic Term Mass: " << topicTermMass << endl;
        ss << "Topic Term Scores: ";
        for (topic_t ind = 0; ind < currentTypeTopicCounts->length; ind++) {
            ss << topic_term_scores[ind] << ", ";
        }
        ss << endl;
        ss << "Tokens Per Topic: ";
        for (topic_t ind = 0; ind < numTopics; ind++) {
            ss << tokens_per_topic[ind] << ", ";
        }
        ss << endl;
        ss << "Word Topic Counts: ";
        for (topic_t ind = 0; ind < currentTypeTopicCounts->length; ind++) {
            ss << "(" << currentTypeTopicCounts->items[ind].choose.top << ", "
                    << currentTypeTopicCounts->items[ind].choose.cnt << ") ";
        }
        ss << endl;
        ss << "Local Topic Counts: ";
        for (topic_t denseIndex = 0; denseIndex < non_zero_topics; denseIndex++) {
            topic_t topic = local_topic_index[denseIndex];
            ss << "(" << topic << ", " << local_topic_counts[topic] << ") ";
        }
        ss << endl;
        ss << "Cached-coefficients: ";
        for (topic_t ind = 0; ind < numTopics; ind++) {
            ss << cachedCoefficients[ind] << ", ";
        }
        ss << endl;
        LOG(FATAL) << ss.str();
        ss.str("");
    }

    //Return the found topic sample
    return new_topic;
}

} // namespace sampler
