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
 * GenericTopKList.h
 *
 *
 *  Created on: 22-Jul-2010
 *      
 */

#ifndef GENERICTOPKLIST_H_
#define GENERICTOPKLIST_H_

#include <queue>
#include <stack>
#include <fstream>

using namespace std;

//!A list that maintains top K elements
template<class T, class GreaterThan>
class GenericTopKList {
private:
    priority_queue<T, vector<T> , GreaterThan> pq;
    size_t K;

public:
    GenericTopKList(size_t K_ = 10) :
        K(K_) {
    }

    virtual ~GenericTopKList() {
    }

    void push(T elem) {
        if (pq.size() < K) {
            pq.push(elem);
        } else {
            T top_elem = pq.top();
            GreaterThan gt;
            if (gt(elem, top_elem)) {
                pq.pop();
                pq.push(elem);
            }
        }
    }

    T top() {
        return pq.top();
    }

    void pop() {
        if (!pq.empty())
            pq.pop();
    }

    bool empty() {
        return pq.empty();
    }

    void print(ostream& out) {
        stack<T> st;
        while (!pq.empty()) {
            st.push(pq.top());
            pq.pop();
        }
        while (!st.empty()) {
            out << st.top() << ",";
            st.pop();
        }
    }

    void clear() {
        while (!pq.empty())
            pq.pop();
    }
};

#endif /* GENERICTOPKLIST_H_ */
