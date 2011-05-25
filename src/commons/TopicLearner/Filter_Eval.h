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
 * Filter_Eval.cpp
 *
 *
 *
 *  Created on: 24 Mar, 2009
 *      
 */

#ifndef FILTER_EVAL_H_
#define FILTER_EVAL_H_

#include "tbb/pipeline.h"
#include "Model_Refiner.h"

using namespace tbb;
using namespace std;

//!A filter in the TBB pipeline.
/**
 * Delegates the task to be done to
 * refiner.eval. The output from the eval step is accumulated and
 * can be queried using get_eval()
 */
class Filter_Eval: public filter {
private:
    Model_Refiner& _refiner;
    double _value;

public:
    Filter_Eval(Model_Refiner&);
    virtual ~Filter_Eval();

    void* operator()(void*);
    double get_eval();
};

#endif /* FILTER_EVAL_H_ */
