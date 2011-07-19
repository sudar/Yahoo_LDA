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
