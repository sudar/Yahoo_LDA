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
