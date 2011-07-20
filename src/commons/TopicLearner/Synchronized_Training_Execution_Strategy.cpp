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
 * Synchronized_Training_Execution_Strategy.cpp
 *
 *  Created on: 07-Jan-2011
 *      
 */

#include "Synchronized_Training_Execution_Strategy.h"
#include "Synchronizer.h"
#include <pthread.h>
#include <glog/logging.h>

Synchronized_Training_Execution_Strategy::Synchronized_Training_Execution_Strategy(
        Pipeline& pipeline, Model& model, Checkpointer& checkpointer,
        Synchronizer_Helper& sync_helper) :
    Training_Execution_Strategy(pipeline, model, checkpointer),
            _sync_helper(sync_helper) {
}

Synchronized_Training_Execution_Strategy::~Synchronized_Training_Execution_Strategy() {
}

void* synchronize(void* inp) {
    Synchronizer* synchronizer = (Synchronizer*) (inp);
    synchronizer->synchronize();
    return NULL;
}

//Start a synchronizer thread that performs background
//synchronization. Run the usual execution strategy
//Once that is done, indicate the synchronizer to stop
void Synchronized_Training_Execution_Strategy::execute() {
    Synchronizer synchronizer(_sync_helper);
    pthread_t synch_thread;
    pthread_create(&synch_thread, NULL, ::synchronize, &synchronizer);
    Training_Execution_Strategy::execute();
    LOG(WARNING) << "All Iters Done";
    synchronizer.set_all_iters_done();
    void *status;
    pthread_join(synch_thread, &status);
    LOG(WARNING) << "All threads joined";
}
