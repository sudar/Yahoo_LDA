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
#include "Filter_Reader.h"
#include "Context.h"

Filter_Reader::Filter_Reader(Model_Refiner& refiner) :
    filter(serial), _refiner(refiner) {
    int livetokens = Context::get_instance().get_int("livetokens");
    docs = _refiner.allocate_document_buffer(livetokens);
    next_doc = 0;
}

Filter_Reader::~Filter_Reader() {
    _refiner.deallocate_document_buffer(docs);
}

void* Filter_Reader::operator ()(void *) {
    int livetokens = Context::get_instance().get_int("livetokens");
    google::protobuf::Message* doc = _refiner.get_nth_document(docs, next_doc);
    next_doc = (next_doc + 1) % livetokens;
    return _refiner.read(*doc);
}
