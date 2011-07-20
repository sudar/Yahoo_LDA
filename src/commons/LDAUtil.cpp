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
