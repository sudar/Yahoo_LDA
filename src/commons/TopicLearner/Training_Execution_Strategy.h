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
