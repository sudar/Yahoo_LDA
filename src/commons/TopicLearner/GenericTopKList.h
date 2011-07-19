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
