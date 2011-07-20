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
