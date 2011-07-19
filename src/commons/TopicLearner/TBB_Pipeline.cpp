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
 * TBB_Pipeline.cpp
 *
 *  Created on: 04-Jan-2011
 *      
 */

#include "TBB_Pipeline.h"
#include "Filter_Reader.h"
#include "Filter_Sampler.h"
#include "Filter_Updater.h"
#include "Filter_Optimizer.h"
#include "Filter_Eval.h"
#include "Filter_Writer.h"
#include "Filter_Tester.h"
#include "Context.h"

TBB_Pipeline::TBB_Pipeline(Model_Refiner& refiner) :
    _refiner(refiner) {
    Context& context = Context::get_instance();
    int num_threads = context.get_int("samplerthreads");
    _init.initialize(num_threads);
}

TBB_Pipeline::~TBB_Pipeline() { }

void TBB_Pipeline::init() {
    _pipeline = new tbb::pipeline;
    _reader = new Filter_Reader(_refiner);
    _sampler = new Filter_Sampler(_refiner);
    _updater = new Filter_Updater(_refiner);
    _optimizer = new Filter_Optimizer(_refiner);
    _eval = new Filter_Eval(_refiner);
    _writer = new Filter_Writer(_refiner);
    _tester = new Filter_Tester(_refiner);
}

void TBB_Pipeline::destroy() {
    delete _pipeline;
    delete _reader;
    delete _sampler;
    delete _updater;
    delete _optimizer;
    delete _eval;
    delete _writer;
    delete _tester;
}

void TBB_Pipeline::clear() {
    _refiner.iteration_done();
    _pipeline->clear();
}

void TBB_Pipeline::add_reader() {
    _pipeline->add_filter(*_reader);
}

void TBB_Pipeline::add_sampler() {
    _pipeline->add_filter(*_sampler);
}

void TBB_Pipeline::add_updater() {
    _pipeline->add_filter(*_updater);
}

void TBB_Pipeline::add_optimizer() {
    _pipeline->add_filter(*_optimizer);
}

void TBB_Pipeline::add_eval() {
    _pipeline->add_filter(*_eval);
}

void TBB_Pipeline::add_writer() {
    _pipeline->add_filter(*_writer);
}

void TBB_Pipeline::add_tester() {
    _pipeline->add_filter(*_tester);
}

Model_Refiner& TBB_Pipeline::get_refiner() {
    return _refiner;
}

double TBB_Pipeline::get_eval() {
    Filter_Eval* eval = dynamic_cast<Filter_Eval*> (_eval);
    return eval->get_eval();
}

void TBB_Pipeline::run() {
    _pipeline->run(Context::get_instance().get_int("livetokens"));
}
