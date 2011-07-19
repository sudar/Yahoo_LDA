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
 * StringTokenizer.cpp
 *
 *  Created on: 20-Oct-2010
 *      
 */

#include "LDAUtil.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>

std::string LDAUtil::Itoa::_cache[100] = { "0", "1", "2", "3", "4", "5", "6",
        "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18",
        "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
        "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42",
        "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54",
        "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66",
        "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78",
        "79", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90",
        "91", "92", "93", "94", "95", "96", "97", "98", "99" };
std::stringstream LDAUtil::Itoa::_ss;

namespace LDAUtil {

void StringTokenizer::tokenize(const string& s, char delim,
        vector<string>& ret_vec) {
    ret_vec.clear();
    size_t start_pos = 0;
    size_t sz = s.size();
    while (start_pos < sz) {
        size_t pos = s.find_first_of(delim, start_pos);
        if (pos == string::npos)
            pos = sz;
        string sub_str = s.substr(start_pos, pos - start_pos);
        StringTrimmer::trim(sub_str);
        ret_vec.push_back(sub_str);
        start_pos = pos + 1;
    }
}

StringTokenizer::StringTokenizer() {
}

StringTokenizer::~StringTokenizer() {
}

void StringTrimmer::trim(string& s, char trim_char) {
    size_t start_pos = 0;
    while (true) {
        size_t pos = s.find_first_of(trim_char, start_pos);
        if (pos == string::npos)
            break;
        s.erase(pos, 1);
        start_pos = pos;
    }
}

StringTrimmer::StringTrimmer() {
}

StringTrimmer::~StringTrimmer() {
}

string DM_Server_Names::get_servant_name(const int server_id) {
    return "DM_Server_" + Itoa::get_string(server_id);
}

string DM_Server_Names::get_server_endpoint(const string& host_port) {
    vector<string> host;
    StringTokenizer::tokenize(host_port, ':', host);
    return "default -h " + host[0] + " -p " + host[1];
}

Itoa::Itoa() {
}

Itoa::~Itoa() {
}

string Itoa::get_string(const int i) {
    if (0 <= i && i <= 99)
        return _cache[i];
    else {
        _ss.str("");
        _ss << i;
        return _ss.str();
    }
}

}
/*using namespace std;

 int main(int argc, char* argv[]){
 string test = "123,45,6789,";
 vector<string> tokens;
 LDAUtil::StringTokenizer::tokenize(test,',',tokens);
 ostream_iterator<string> out_it(cout, " ");
 copy(tokens.begin(),tokens.end(),out_it);
 cout << endl;
 test = " 123 45 6789 ";
 LDAUtil::StringTrimmer::trim(test);
 cout << test;
 }*/
