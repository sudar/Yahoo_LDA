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
