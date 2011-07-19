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
