/*******************************************************************************
    Copyright (c) 2011, Yahoo! Inc.
    All rights reserved.

    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:

    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.

    * Neither the name of Yahoo! Inc. nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of Yahoo! Inc.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    The Initial Developer of the Original Code is Shravan Narayanamurthy.
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
