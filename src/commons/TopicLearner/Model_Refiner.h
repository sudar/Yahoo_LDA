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
 * Model_Refiner.h
 *
 *
 *
 *  Created on: 23-Dec-2010
 *      
 */

#ifndef MODEL_REFINER_H_
#define MODEL_REFINER_H_

#include "google/protobuf/message.h"

//!An interface that defines the necessary refinements
//!for refining the LDA graphical model and its extensions.
/**
 * A refinement is defined as an operation done to improve
 * the model like sampling. The definition is also extended
 * to operations that enable a refinement like reading documents
 * and evaluating log-likelihoods.
 *
 * The provider of a model's implementation has to implement
 * this interface suitably
 */
class Model_Refiner {
public:
    virtual google::protobuf::Message* allocate_document_buffer(size_t) = 0;
    virtual void deallocate_document_buffer(google::protobuf::Message*) = 0;
    virtual google::protobuf::Message* get_nth_document(
            google::protobuf::Message* docs, size_t n) = 0;
    virtual void* read(google::protobuf::Message&) = 0;
    virtual void* sample(void*) = 0;
    virtual void* test(void*) = 0;
    virtual void* update(void*) = 0;
    virtual void* optimize(void*) = 0;
    virtual void* eval(void*, double&) = 0;
    virtual void write(void*) = 0;
    virtual void iteration_done() = 0;
};

#endif /* MODEL_REFINER_H_ */
