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
 * Unigram_Model_Synchronized_Training_Builder.h
 *
 *  Created on: 14-Jan-2011
 *      
 */

#ifndef UNIGRAM_MODEL_SYNCHRONIZED_TRAINING_BUILDER_H_
#define UNIGRAM_MODEL_SYNCHRONIZED_TRAINING_BUILDER_H_

#include "Unigram_Model_Training_Builder.h"
#include "TopicLearner/Synchronizer_Helper.h"

class Unigram_Model_Synchronized_Training_Builder: public Unigram_Model_Training_Builder {
public:
    Unigram_Model_Synchronized_Training_Builder();
    virtual ~Unigram_Model_Synchronized_Training_Builder();

    virtual Execution_Strategy& create_execution_strategy(Pipeline&);

protected:
    Synchronizer_Helper* _sync_helper;
};

#endif /* UNIGRAM_MODEL_SYNCHRONIZED_TRAINING_BUILDER_H_ */
