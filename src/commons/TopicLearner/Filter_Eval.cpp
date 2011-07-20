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
#include "Filter_Eval.h"

Filter_Eval::Filter_Eval(Model_Refiner& refiner) :
    filter(serial), _refiner(refiner) {
    _value = 0;
}

Filter_Eval::~Filter_Eval() {
}

void* Filter_Eval::operator ()(void *token) {
    double value = 0;
    void* ret_token = _refiner.eval(token, value);
    _value += value;
    return ret_token;
}

double Filter_Eval::get_eval() {
    return _value;
}
