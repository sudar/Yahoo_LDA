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
 * Dirichlet.h
 * The log_gamma digamma functions declarations
 *  Created on: 13 May, 2009
 *      
 */

#ifndef DIRICHLET_H_
#define DIRICHLET_H_

#include "constants.h"

using namespace boost;

double log_gamma(double z);
double digamma(double z);

#endif /* DIRICHLET_H_ */
