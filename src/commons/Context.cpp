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
 * Context.cpp
 *
 *
 *  Created on: 23-Dec-2010
 *      
 */

#include "Context.h"
#include "gflags/gflags.h"
#include <vector>
#include <cstdlib>
#include "glog/logging.h"

Context::Context() {
    std::vector<google::CommandLineFlagInfo> flags;
    google::GetAllFlags(&flags);
    for (size_t i = 0; i < flags.size(); i++) {
        google::CommandLineFlagInfo& flag = flags[i];
        _flags[flag.name] = flag.is_default ? flag.default_value
                : flag.current_value;
    }
}

Context::~Context() {
}

Context& Context::get_instance() {
    if (_instance)
        return *_instance;
    _instance = new Context();
    return *_instance;
}

string Context::get_string(string key) {
    if (_flags.count(key))
        return _flags[key];
    string err = key + " not found. Invalid access";
    LOG(WARNING) << err;
    throw err;
}

int Context::get_int(string key) {
    return atoi(get_string(key).c_str());
}

double Context::get_double(string key) {
    return atof(get_string(key).c_str());
}

bool Context::get_bool(string key) {
    if (get_string(key).compare("true") == 0)
        return true;
    else
        return false;
}

void Context::put_string(string key, string val) {
    _flags[key] = val;
}

Context* Context::_instance = NULL;
