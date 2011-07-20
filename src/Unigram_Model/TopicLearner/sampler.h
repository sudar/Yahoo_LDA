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
