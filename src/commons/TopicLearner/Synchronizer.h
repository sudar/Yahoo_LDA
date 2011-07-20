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
 * Synchronizer.h
 *
 *
 *  Created on: 07-Jan-2011
 *      
 */

#ifndef SYNCHRONIZER_H_
#define SYNCHRONIZER_H_

#include "Synchronizer_Helper.h"

//!The synchronization strategy that
//!uses the Synchronizer_Helper to perform the
//!concrete synchronization steps.
/**
 * It provides slots, so to say, for synchronization
 * and calls the Synchronizer_Helper to perform its
 * duties for that slot.
 *
 * This greatly reduces the burden on the model writer
 *
 * So whenever a model needs to be scaled by providing
 * a distributed inferencing solution, all the model writer
 * has to do is write a Synchronizer_Helper.
 *
 * This is a default synchronization strategy provided and
 * users can implement their own strategy by extending this
 *
 * The main aim is to run the inferencing locally while
 * keeping the model in sync globally through the use of
 * this Synchronizer and a Distributed Map
 */
class Synchronizer {
public:
    Synchronizer(Synchronizer_Helper&);
    virtual ~Synchronizer();

    //!The synchronization strategy
    void synchronize();

    //!Set that all iterations of the Execution_Strategy
    //!have been done
    void set_all_iters_done();

    //!Check if the Execution_Strategy has finished
    bool is_all_iters_done();

private:
    Synchronizer_Helper& _sync_helper;
    bool _all_iters_done;

};

#endif /* SYNCHRONIZER_H_ */
