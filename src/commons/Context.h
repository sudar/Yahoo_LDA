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
 * Context.h
 *
 *
 *  Created on: 23-Dec-2010
 *      
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "boost/unordered_map.hpp"
#include <string>

using namespace std;
using namespace boost;

//!An object that maintains the context for the executing code
/**
 * This is a Singleton object that maintains a context
 * for the executing code. It contains a list of properties
 * stored as a key-value map
 *
 * All the flags defined are made available through this
 * object. It can also be used as a mechanism for message
 * passing. This reduces coupling between the code and
 * gflags.
 */
class Context {
public:
    static Context& get_instance();

    //!Get an integer valued property named key
    int get_int(string key);
    //!Get a string valued property named key
    string get_string(string key);
    //!Get a double valued property named key
    double get_double(string key);
    //!Get a bool valued property named key
    bool get_bool(string key);

    //!Put a string value for property key
    //!into the map
    void put_string(string key, string val);

private:
    Context();
    virtual ~Context();

    static Context* _instance;
    unordered_map<string, string> _flags;
};

#endif /* CONTEXT_H_ */
