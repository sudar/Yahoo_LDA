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
 * Data_Formatter.h
 *
 *
 *  Created on: 11-Jan-2011
 *      
 */

#ifndef DATA_FORMATTER_H_
#define DATA_FORMATTER_H_

#include "WordIndexDictionary.h"

//!An interface for formatter objects
/** A formatter is an object that
 * converts raw text corpus into
 * binary so that its disk footprint
 * is low and there is no parsing
 * involved while reading it back
 */
class Data_Formatter {
public:
    //!Perform the actual formatting
    virtual void format() = 0;

    //!Return the dictionary being used by the formatter
    virtual WordIndexDictionary& get_dictionary() = 0;

    //!The number of documents formatted
    virtual int get_num_docs() = 0;

    //!The total number of words found
    virtual int get_total_num_words() = 0;
};

#endif /* DATA_FORMATTER_H_ */
