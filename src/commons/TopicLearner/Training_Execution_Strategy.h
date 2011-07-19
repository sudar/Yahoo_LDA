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
