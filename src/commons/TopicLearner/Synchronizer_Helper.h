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
 * Synchronizer_Helper.h
 *
 *
 *  Created on: 07-Jan-2011
 *      
 */

#ifndef SYNCHRONIZER_HELPER_H_
#define SYNCHRONIZER_HELPER_H_

#include <string>

//!A helper class for the synchronizer
/**
 * Each helper object implements synchronization algorithms
 * for maintaining the data structures in sync
 *
 * The Synchronizer class depends on this interface to enable
 * synchronization of a multi-machine LDA set-up
 */
class Synchronizer_Helper {
public:
    virtual void initialize() = 0;
    virtual void synchronize() = 0;

    //!Returns true as long as all items to be
    //!synchronized are not synchronized
    virtual bool has_to_synchronize() = 0;

    //!After this call, has_to_synchronize()
    //!should return true
    virtual void reset_to_synchronize() = 0;

    //!This is a callback from the Client
    //!when an async_putNget is used on the Client
    //!So when a Client is instantiated, you need
    //!to pass a reference of (*this)
    virtual void
            end_putNget(const std::string& word, const std::string& counts) = 0;
};

#endif /* SYNCHRONIZER_HELPER_H_ */
