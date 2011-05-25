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
