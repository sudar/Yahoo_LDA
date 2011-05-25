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
 * Filter_Tester.cpp
 *
 *
 *
 *  Created on: 6 Apr, 2009
 *      
 */

#ifndef FILTER_TESTER_H_
#define FILTER_TESTER_H_

#include "tbb/pipeline.h"
#include "Model_Refiner.h"

using namespace tbb;
//!A filter in the TBB pipeline.
/**
 * Delegates the task to be done to
 * refiner.test()
 */
class Filter_Tester: public filter {
private:
    Model_Refiner& _refiner;

public:
    Filter_Tester(Model_Refiner&);
    virtual ~Filter_Tester();

    void* operator ()(void *);
};

#endif /* FILTER_TESTER_H_ */
