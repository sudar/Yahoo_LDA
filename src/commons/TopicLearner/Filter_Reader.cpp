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
