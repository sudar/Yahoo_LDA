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
 * Local_Checkpointer.h
 *
 *  Created on: 07-Mar-2011
 *      
 */

#ifndef LOCAL_CHECKPOINTER_H_
#define LOCAL_CHECKPOINTER_H_

#include "TopicLearner/Checkpointer.h"

class Local_Checkpointer: public Checkpointer {
public:
    Local_Checkpointer();
    virtual ~Local_Checkpointer();

    virtual void save_metadata(std::string& state);
    virtual std::string load_metadata();
    virtual void checkpoint();
};

#endif /* LOCAL_CHECKPOINTER_H_ */
