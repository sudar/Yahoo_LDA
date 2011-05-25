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
 * Execution_Strategy.h
 *
 *
 *
 *  Created on: 23-Dec-2010
 *      
 */

#ifndef EXECUTION_STRATEGY_H_
#define EXECUTION_STRATEGY_H_

//!Interface for strategy objects
/**
 * Implement this interface to define
 * a strategy detailing what filters
 * are added to the pipeline and how
 * to run the pipeline
 */
class Execution_Strategy {
public:
    virtual void execute() = 0;
};

#endif /* EXECUTION_STRATEGY_H_ */
