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
 * Synchronizer.cpp
 *
 *  Created on: 07-Jan-2011
 *      
 */

#include "Synchronizer.h"
#include "tbb/tick_count.h"
#include "types.h"
#include "glog/logging.h"

Synchronizer::Synchronizer(Synchronizer_Helper& sync_helper) :
    _sync_helper(sync_helper) {
    _all_iters_done = false;
    _sync_helper.initialize();
}

Synchronizer::~Synchronizer() { }

void Synchronizer::set_all_iters_done() {
    _all_iters_done = true;
}

bool Synchronizer::is_all_iters_done() {
    return _all_iters_done;
}

void Synchronizer::synchronize() {
    LOG(WARNING) << "Starting synchronization";
    int sync_time_cnt = 0;
    double sync_time = 0.0;
    do {
        using namespace tbb;
        TIME(t1);
        while (_sync_helper.has_to_synchronize()) {
            _sync_helper.synchronize();
        }
        TIME(t2);
        sync_time += (t2 - t1).seconds();
        ++sync_time_cnt;

        LOG(WARNING) << "Synch pass " << sync_time_cnt << " done. Took "
                << (t2 - t1).seconds() << " seconds";

        _sync_helper.reset_to_synchronize();
    } while (!is_all_iters_done());
    LOG(WARNING)
            << "Stopping synchronization since all iterations are done";
}
