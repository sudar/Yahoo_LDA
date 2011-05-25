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
