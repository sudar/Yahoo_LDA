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
