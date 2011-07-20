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
#include "Filter_Optimizer.h"

//Constructs an optimizer filter
Filter_Optimizer::Filter_Optimizer(Model_Refiner& refiner) :
    filter(serial), _refiner(refiner) {
}

Filter_Optimizer::~Filter_Optimizer() {
}

/**
 * Each document is passed through the filter using this method
 * If we have read in tau documents, we update the global alphas
 * Else we compute gradients & accumulate them in the local alphas
 *
 */
void* Filter_Optimizer::operator ()(void *token) {
    return _refiner.optimize(token);
}
