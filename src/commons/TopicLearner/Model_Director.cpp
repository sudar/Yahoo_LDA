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
 * Model_Director.cpp
 *
 *  Created on: 28-Dec-2010
 *      
 */

#include "Model_Director.h"
#include "Model_Builder.h"

Model_Director::Model_Director() {
}

Model_Director::~Model_Director() {
}

Model& Model_Director::build_model(Model_Builder& builder) {
    Model_Refiner& refiner = builder.create_model_refiner();
    Pipeline& pipeline = builder.create_pipeline(refiner);
    Execution_Strategy& strategy = builder.create_execution_strategy(pipeline);
    strategy.execute();
    return builder.get_model();
}
