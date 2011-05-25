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
 * Unigram_Model_Synchronized_Training_Builder.cpp
 *
 *  Created on: 14-Jan-2011
 *      
 */

#include "Unigram_Model_Synchronized_Training_Builder.h"
#include "Unigram_Model_Synchronizer_Helper.h"
#include "TopicLearner/Synchronized_Training_Execution_Strategy.h"
#include "Hadoop_Checkpointer.h"
#include "Context.h"

Unigram_Model_Synchronized_Training_Builder::Unigram_Model_Synchronized_Training_Builder() {
    _sync_helper = NULL;
}

Unigram_Model_Synchronized_Training_Builder::~Unigram_Model_Synchronized_Training_Builder() {
    if (_sync_helper)
        delete _sync_helper;
}

Execution_Strategy& Unigram_Model_Synchronized_Training_Builder::create_execution_strategy(
        Pipeline& pipeline) {
    _sync_helper = new Unigram_Model_Synchronizer_Helper(_model->get_ttc(),
            get_dict());
    string chkpt_dir = Context::get_instance().get_string("chkptdir");
    if (chkpt_dir == "")
        LOG(FATAL) << "HDFS Checkpoint Directory missing";
    _checkpointer = new Hadoop_Checkpointer();
    _strategy = new Synchronized_Training_Execution_Strategy(pipeline, *_model,
            *_checkpointer, *_sync_helper);
    return *_strategy;
}
