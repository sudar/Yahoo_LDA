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
 * LDAUtil.h
 *
 *
 *
 *  Created on: 20-Oct-2010
 *      
 */

#ifndef LDAUTIL_H_
#define LDAUTIL_H_

#include <vector>
#include <string>
#include <sstream>

namespace LDAUtil {
using namespace std;

/**
 * Utility class for tokenization and such other
 * commonly used functions
 */
class StringTokenizer {
public:
    static void tokenize(const string& s, char delim, vector<string>& ret_vec);

private:
    StringTokenizer();
    virtual ~StringTokenizer();
};

class StringTrimmer {
public:
    static void trim(string& s, char trim_char = ' ');
private:
    StringTrimmer();
    virtual ~StringTrimmer();
};

class Itoa {
public:
    static string get_string(const int i);
private:
    static string _cache[100];
    static stringstream _ss;

    Itoa();
    virtual ~Itoa();
};

class DM_Server_Names {
public:
    static string get_servant_name(const int server_id);
    static string get_server_endpoint(const string& host_port);

private:
    DM_Server_Names();
    virtual ~DM_Server_Names();
};
}

#endif /* LDAUTIL_H_ */
