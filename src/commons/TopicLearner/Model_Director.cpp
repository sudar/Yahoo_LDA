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
