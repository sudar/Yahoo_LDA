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
