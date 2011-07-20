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
 * Model_Director.h
 *
 *
 *
 *
 *  Created on: 28-Dec-2010
 *      
 */

#ifndef MODEL_DIRECTOR_H_
#define MODEL_DIRECTOR_H_

#include "Model_Builder.h"
#include "Model.h"

//!The Director class of the Builder pattern
/**
 * Simple steps: Use the builder to
 * 1. Create Refiner
 * 2. Create a pipeline of filters to perform the
 *    various refinements defined by the Refiner
 * 3. Create a strategy to execute the pipeline of filters
 * 4. Execute the strategy
 * 5. Return the model inside the builder that has been refined
 *    by the strategy
 */
class Model_Director {
public:
    Model_Director();
    virtual ~Model_Director();

    Model& build_model(Model_Builder& builder);
};

#endif /* MODEL_DIRECTOR_H_ */
