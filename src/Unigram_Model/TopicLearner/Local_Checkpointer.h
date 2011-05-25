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
