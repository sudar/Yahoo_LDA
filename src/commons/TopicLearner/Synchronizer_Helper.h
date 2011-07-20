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
