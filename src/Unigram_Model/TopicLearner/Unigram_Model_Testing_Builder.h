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
 * Unigram_Model_Builder.h
 *
 *  Created on: 06-Jan-2011
 *      
 */

#ifndef UNIGRAMMODELTESTINGBUILDER_H_
#define UNIGRAMMODELTESTINGBUILDER_H_

#include "Unigram_Model_Training_Builder.h"

class Unigram_Model_Testing_Builder: public Unigram_Model_Training_Builder {
public:
    Unigram_Model_Testing_Builder();
    virtual ~Unigram_Model_Testing_Builder();
    virtual Model_Refiner& create_model_refiner();
    virtual Execution_Strategy& create_execution_strategy(Pipeline&);
};

#endif /* UNIGRAMMODELTESTINGBUILDER_H_ */
