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
 * TBB_Pipeline.h
 *
 *
 *  Created on: 04-Jan-2011
 *      
 */

#ifndef TBB_PIPELINE_H_
#define TBB_PIPELINE_H_

#include "Pipeline.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/pipeline.h"
#include "Model_Refiner.h"

using namespace tbb;

//!An implementation of the Pipeline interface using
//!Intel's Threading Building Blocks.
/**
 * TBB::filter is the basic unit of computation and TBB::Pipeline
 * puts the filters together.
 *
 * What filters exist in a pipeline and how the pipeline
 * is executed is implemented via an Execution Strategy
 *
 * Calling each of the add methods adds that particular
 * filter to the pipeline
 */
class TBB_Pipeline: public Pipeline {
public:
    TBB_Pipeline(Model_Refiner&);
    virtual ~TBB_Pipeline();
    void init();
    void add_reader();
    void add_sampler();
    void add_updater();
    void add_optimizer();
    void add_eval();
    void add_writer();
    void add_tester();
    void clear();
    void destroy();
    void run();

    Model_Refiner& get_refiner();
    double get_eval();

protected:
    task_scheduler_init _init;
    tbb::pipeline* _pipeline;
    Model_Refiner& _refiner;

    filter *_reader, *_sampler, *_updater, *_optimizer, *_eval, *_writer,
            *_tester;
};

#endif /* TBB_PIPELINE_H_ */
