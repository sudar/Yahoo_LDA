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
 * Model_Builder.h
 *
 *
 *
 *
 *
 *  Created on: 11-May-2010
 *      
 */

#ifndef MODELBUILDER_H_
#define MODELBUILDER_H_

#include "Model_Refiner.h"
#include "Pipeline.h"
#include "Execution_Strategy.h"
#include "Model.h"

//!The builder class which builds the different components
//!needed to create the model.
/**
 * We use the Builder pattern.
 *
 * The components are:
 * 1. Model_Refiner: This main component that describes how
 *                   the model should be refined as documents
 *                   pass through the pipeline
 * 2. Pipeline: A pipeline of filters that perform the various
 *              refinements defined by the refiner
 * 3. Execution_Strategy: A strategy that defines how the pipeline
 *                        of filters has to be executed
 *
 * Usually, different modes demand different builders and similar
 * or different components. So keep in mind that inheritance can
 * be used here to maximize code reuse
 */
class Model_Builder {

public:
    virtual Model_Refiner& create_model_refiner() = 0;
    virtual Pipeline& create_pipeline(Model_Refiner&) = 0;
    virtual Execution_Strategy& create_execution_strategy(Pipeline&) = 0;
    virtual void create_output() = 0;

    virtual Model& get_model() = 0;
};

#endif /* MODELBUILDER_H_ */
