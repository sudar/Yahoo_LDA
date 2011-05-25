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
 * FormatData_flags_define.h
 *
 * Contains definitions of formatter related flags
 *
 *  Created on: 19 Jul, 2009
 *      
 */

#ifndef FORMATDATA_FLAGS_DEFINE_H_
#define FORMATDATA_FLAGS_DEFINE_H_

#include "gflags/gflags.h"

DEFINE_int32(model,1,"Unigram-1 or some other model");
DEFINE_string(corpus,"specify","A text file with space separated fields containing doc-id, category, contents of the doc");
DEFINE_string(outputprefix,"lda", "A prefix that will be used with all files output by the program");
DEFINE_string(dumpfile,"specify", "The dump of the dictionary to be used instead of creating afresh");

#endif /* FORMATDATA_FLAGS_DEFINE_H_ */
