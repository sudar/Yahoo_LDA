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
 * Checkpointer.h
 *
 *
 *
 *  Created on: 07-Mar-2011
 *      
 */

#ifndef CHECKPOINTER_H_
#define CHECKPOINTER_H_

#include <string>

//!Used to implement failure recovery
/**
 * The checkpointer interface.
 * A checkpointer object helps create a checkpoint and
 * also to load from a checkpoint
 *
 * A checkpointer has two things:
 * 1. Metadata - (Ex.: The iteration at which this checkpoint was created)
 * 2. The method to serialize the necessary data structures to disk
 *
 */
class Checkpointer {
public:
    //!Serialize the metadata
    virtual void save_metadata(std::string& state) = 0;
    //!Load the metadata
    virtual std::string load_metadata() = 0;
    //!Serialize other necessary data structures
    virtual void checkpoint() = 0;
};

#endif /* CHECKPOINTER_H_ */
