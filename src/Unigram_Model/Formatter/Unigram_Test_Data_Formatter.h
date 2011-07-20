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
 * Unigram_Test_Data_Formatter.h
 *
 *  Created on: 28-Jan-2011
 *      
 */

#ifndef UNIGRAM_TEST_DATA_FORMATTER_H_
#define UNIGRAM_TEST_DATA_FORMATTER_H_

#include "Unigram_Train_Data_Formatter.h"

class Unigram_Test_Data_Formatter: public Unigram_Train_Data_Formatter {
public:
    Unigram_Test_Data_Formatter();
    virtual ~Unigram_Test_Data_Formatter();

protected:
    int insert_word_to_dict(std::string word);

private:
    WordIndexDictionary _global_dict;
};

#endif /* UNIGRAM_TEST_DATA_FORMATTER_H_ */
