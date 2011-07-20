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
