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
