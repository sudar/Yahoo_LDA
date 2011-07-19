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
