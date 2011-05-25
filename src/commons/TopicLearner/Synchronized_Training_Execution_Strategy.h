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
 * Synchronized_Training_Execution_Strategy.h
 *
 *  Created on: 07-Jan-2011
 *      
 */

#ifndef SYNCHRONIZED_TRAINING_EXECUTION_STRATEGY_H_
#define SYNCHRONIZED_TRAINING_EXECUTION_STRATEGY_H_

#include "Training_Execution_Strategy.h"
#include "Synchronizer_Helper.h"

//!Default implementation of the Execution_Strategy interface
/**
 * Runs the Training_Execution_Strategy at its
 * core and also provides for synchronization of the model
 */
class Synchronized_Training_Execution_Strategy: public Training_Execution_Strategy {
public:
    Synchronized_Training_Execution_Strategy(Pipeline&, Model&, Checkpointer&,
            Synchronizer_Helper&);
    virtual ~Synchronized_Training_Execution_Strategy();

    void execute();

private:
    Synchronizer_Helper& _sync_helper;
};

#endif /* SYNCHRONIZED_TRAINING_EXECUTION_STRATEGY_H_ */
