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
 * Unigram_Model_Streaming_Builder.h
 *
 *  Created on: 29-Mar-2011
 *      
 */
#ifndef UNIGRAM_MODEL_STREAMING_BUILDER_H_
#define UNIGRAM_MODEL_STREAMING_BUILDER_H_

#include "TopicLearner/Model_Builder.h"
#include "Unigram_Model.h"
#include "WordIndexDictionary.h"

class Unigram_Model_Streaming_Builder: public Model_Builder {
public:
    Unigram_Model_Streaming_Builder();
    virtual ~Unigram_Model_Streaming_Builder();
    virtual Model_Refiner& create_model_refiner();
    virtual Pipeline& create_pipeline(Model_Refiner&);
    virtual Execution_Strategy& create_execution_strategy(Pipeline&);

    void create_output();
    Model& get_model();

protected:
    void init_dict();
    WordIndexDictionary& get_dict();

protected:
    Unigram_Model* _model;
    WordIndexDictionary* _dict;
    WordIndexDictionary* _global_dict;
    Model_Refiner* _refiner;
    Pipeline* _pipeline;
    Execution_Strategy* _strategy;
};

#endif /* UNIGRAM_MODEL_STREAMING_BUILDER_H_ */
