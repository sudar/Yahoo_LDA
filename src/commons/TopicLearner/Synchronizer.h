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
