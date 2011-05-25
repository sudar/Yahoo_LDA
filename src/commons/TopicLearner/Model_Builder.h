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
