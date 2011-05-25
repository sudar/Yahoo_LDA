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
