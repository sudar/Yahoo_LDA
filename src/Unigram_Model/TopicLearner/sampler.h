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
 * sampler.h
 *
 * The sampling function which samples the
 * new topic assignment using the collapsed
 * Gibbs Sampler approach. It takes in the
 * topic counts for the current word, the
 * local topic counts for the document being
 * processed and Abar, Bbar & Ccached
 *
 * The procedure is simple. It computes C(t)
 * and Cbar. It then generates a
 * random number using the uniform RNG passed
 * in. It scales the number by Abar + Bbar + Cbar
 * It checks to which probability mass the number
 * generated belongs. Based on that samples the
 * topic responsible for generating this number
 *
 *  Created on: 24 Apr, 2009
 *      
 */

#ifndef SAMPLER_H_
#define SAMPLER_H_

#include "constants.h"
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>
#include "tbb/atomic.h"
#include "TopicCounts.h"

using namespace boost;
using namespace tbb;
namespace sampler {

topic_t
        sample(
                const topicCounts* currentTypeTopicCounts,
                const topic_t old_topic,
                const atomic<topic_t>* tokens_per_topic,
                const topic_t* loca_topic_counts,
                const topic_t* loca_topic_index,
                const int& non_zero_topics,
                const double& smoothingOnlyMass,
                const double& topicBetaMass,
                const double* cachedCoefficients,
                const double& betaSum,
                const double* alpha,
                double* topic_term_scores,
                const topic_t& numTopics,
                variate_generator<base_generator_type&, boost::uniform_real<> >* unif01);

} // namespace sampler
#endif /* SAMPLER_H_ */
