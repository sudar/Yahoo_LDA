/*******************************************************************************
    Copyright (c) 2011, Yahoo! Inc.
    All rights reserved.

    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:

    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.

    * Neither the name of Yahoo! Inc. nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of Yahoo! Inc.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    The Initial Developer of the Original Code is Shravan Narayanamurthy.
******************************************************************************/
/*
 * Unigram_Model_Builder.h
 *
 *  Created on: 06-Jan-2011
 *      
 */

#ifndef UNIGRAMMODELTRAININGBUILDER_H_
#define UNIGRAMMODELTRAININGBUILDER_H_

#include "TopicLearner/Model_Builder.h"
#include "Unigram_Model.h"
#include "WordIndexDictionary.h"
#include "TopicLearner/Checkpointer.h"

class Unigram_Model_Training_Builder: public Model_Builder {
public:
    Unigram_Model_Training_Builder();
    virtual ~Unigram_Model_Training_Builder();
    virtual Model_Refiner& create_model_refiner();
    virtual Pipeline& create_pipeline(Model_Refiner&);
    virtual Execution_Strategy& create_execution_strategy(Pipeline&);

    void create_output();
    Model& get_model();

protected:
    void init_dict();
    WordIndexDictionary& get_dict();
    void initialize_topics(string, string, int);

protected:
    Unigram_Model* _model;
    WordIndexDictionary* _dict;
    Model_Refiner* _refiner;
    Pipeline* _pipeline;
    Checkpointer* _checkpointer;
    Execution_Strategy* _strategy;
};

#endif /* UNIGRAMMODELTRAININGBUILDER_H_ */
