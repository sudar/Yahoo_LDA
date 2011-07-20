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
 * Simple_Execution_Strategy.cpp
 *
 *  Created on: 28-Dec-2010
 *      
 */

#include "Training_Execution_Strategy.h"
#include "Context.h"
#include "tbb/tick_count.h"
#include "glog/logging.h"

Training_Execution_Strategy::Training_Execution_Strategy(Pipeline& pipeline,
        Model& model, Checkpointer& checkpointer) :
    _pipeline(pipeline), _model(model), _checkpointer(checkpointer) {
}

Training_Execution_Strategy::~Training_Execution_Strategy() {
}

void Training_Execution_Strategy::execute() {
    Context& context = Context::get_instance();
    int start_iter = context.get_int("startiter");
    int end_iter = context.get_int("iter");
    int loglikelihood_interval = context.get_int("printloglikelihood");
    int optimize_interval = context.get_int("optimizestats");
    int burnin = context.get_int("burnin");
    int chkpt_interval = context.get_int("chkptinterval");

    //Check if checkpoint metadata is available
    std::string prefix = context.get_string("inputprefix");
    context.put_string("chkpt_file", prefix + ".chk");
    std::string state = _checkpointer.load_metadata();
    if (state.size() > 0)
        start_iter = *((int*) state.c_str()) + 1;

    LOG(WARNING) << "Starting Parallel training Pipeline";
    for (int iter = start_iter; iter <= end_iter; ++iter) {
        bool compute_loglikelihood = (iter == start_iter) || (iter
                % loglikelihood_interval == 0);
        bool optimize = (iter > burnin) && (iter % optimize_interval == 0);
        _pipeline.init();
        _pipeline.add_reader();
        if (!optimize) {
            _pipeline.add_sampler();
            _pipeline.add_updater();
        }
        if (optimize)
            _pipeline.add_optimizer();
        if (compute_loglikelihood)
            _pipeline.add_eval();
        _pipeline.add_writer();
        tbb::tick_count t0 = tbb::tick_count::now();
        _pipeline.run();
        tbb::tick_count t1 = tbb::tick_count::now();
        LOG(WARNING) << "Iteration " << iter << " done. Took "
                << (t1 - t0).seconds() / 60 << " mins" << endl;
        if (compute_loglikelihood) {
            double word_loglikelihood = _model.get_eval();
            double doc_loglikelihood = _pipeline.get_eval();
            LOG(WARNING)
                    << ">>>>>>>>>> Log-Likelihood (model, doc, total): "
                    << word_loglikelihood << " , " << doc_loglikelihood
                    << " , " << word_loglikelihood + doc_loglikelihood;
        }
        _pipeline.clear();
        _pipeline.destroy();
        if (iter % chkpt_interval == 0) {
            LOG(WARNING) << ">>>>>>>>>>> Check Pointing at iteration: "
                    << iter;
            std::string chkpt_state((char*) &iter, sizeof(int));
            _checkpointer.save_metadata(chkpt_state);
            _checkpointer.checkpoint();
        }
    }
    LOG(WARNING) << "Parallel training Pipeline done";
}
