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
 * Pipeline_Builder.h
 *
 *  Created on: 03-Jan-2011
 *      
 */

#ifndef PIPELINE_BUILDER_H_
#define PIPELINE_BUILDER_H_

#include "TopicLearner/Model_Refiner.h"

//!An interface that all pipeline objects must implement
/**
 * A pipeline is a sequence of computation steps
 * that can be performed on some data passing through
 * it. Its similar to an assembly pipeline.
 */
class Pipeline {
public:
    virtual void init() = 0;
    virtual void add_reader() = 0;
    virtual void add_sampler() = 0;
    virtual void add_updater() = 0;
    virtual void add_optimizer() = 0;
    virtual void add_eval() = 0;
    virtual void add_writer() = 0;
    virtual void add_tester() = 0;
    virtual void clear() = 0;
    virtual void destroy() = 0;
    virtual void run() = 0;
    virtual Model_Refiner& get_refiner() = 0;
    virtual double get_eval() = 0;
};

#endif /* PIPELINE_BUILDER_H_ */
