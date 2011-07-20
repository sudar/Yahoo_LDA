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
 * Training_Execution_Strategy.h
 *
 *
 *
 *  Created on: 28-Dec-2010
 *      
 */

#ifndef TRAINING_EXECUTION_STRATEGY_H_
#define TRAINING_EXECUTION_STRATEGY_H_

#include "Execution_Strategy.h"
#include "Pipeline.h"
#include "Model.h"
#include "Checkpointer.h"

/**
 * This is a default implementation for the Execution_Strategy
 * interface for training a model.
 *
 * This can use a passed in Checkpointer to do failure recovery
 *
 * A pipeline needs to be passed.
 */
class Training_Execution_Strategy: public Execution_Strategy {
public:
    Training_Execution_Strategy(Pipeline&, Model&, Checkpointer&);
    virtual ~Training_Execution_Strategy();

    //!Define what filters need to added and when
    //!Runs the pipeline for 'iter' iterations
    void execute();

private:
    Pipeline& _pipeline;
    Model& _model;
    Checkpointer& _checkpointer;
};

#endif /* TRAINING_EXECUTION_STRATEGY_H_ */
