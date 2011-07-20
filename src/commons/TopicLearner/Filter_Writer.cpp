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
#include "Filter_Writer.h"

Filter_Writer::Filter_Writer(Model_Refiner& refiner) :
    filter(serial), _refiner(refiner) {
}

Filter_Writer::~Filter_Writer() {
}

/**
 * We receive the document to write to disk. This contains both words & their
 * topic assignments. We use write_topics() to only write
 * the topics which internally clears the words.
 */
void* Filter_Writer::operator ()(void *token) {
    _refiner.write(token);
    return NULL;
}
