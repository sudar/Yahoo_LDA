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
 * comparator.h
 *
 * Contains various comparator functions used as comparator in stl algorithms
 * All of them implement a descending order on the elements
 *
 *  Created on: 15 Oct, 2009
 *      
 */

#ifndef COMPARATOR_H_
#define COMPARATOR_H_

bool cnt_cmp(packed_t i, packed_t j);

bool cnt_cmp_ttc(cnt_topic_t i, cnt_topic_t j);

bool prob_cmp(tppair v1, tppair v2);

bool freq_cmp(id2freq_t v1, id2freq_t v2);

struct wppair_gt {
    bool operator()(wppair& i, wppair& j) {
        return i.second > j.second;
    }
};

struct bigppair_gt {
    bool operator()(bigppair& i, bigppair& j) {
        return i.second > j.second;
    }
};

#endif /* COMPARATOR_H_ */
