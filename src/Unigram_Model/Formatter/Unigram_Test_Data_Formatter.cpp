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
 * Unigram_Test_Data_Formatter.cpp
 *
 *  Created on: 28-Jan-2011
 *      
 */

#include "Unigram_Test_Data_Formatter.h"
#include "commons/Context.h"

Unigram_Test_Data_Formatter::Unigram_Test_Data_Formatter() {
    Context& context = Context::get_instance();
    string dumpfile = context.get_string("dumpfile");
    LOG(WARNING) << "Initializing Dictionary from " << dumpfile;
    _global_dict.initialize_from_dump(dumpfile);
    LOG(WARNING) << "Num of unique words: " << _global_dict.get_num_words();

    string output_prefix = context.get_string("outputprefix");
    string global_dict_dump = output_prefix + ".dict.dump.global";
    _global_dict.dump(global_dict_dump);
}

Unigram_Test_Data_Formatter::~Unigram_Test_Data_Formatter() {
}

int Unigram_Test_Data_Formatter::insert_word_to_dict(string word) {
    int windex = _global_dict.get_index(word);
    if (windex != -1)
        windex = _dict.insert_word(word);
    return windex;
}
