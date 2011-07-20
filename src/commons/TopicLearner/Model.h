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
 * Model.h
 *
 *
 *  Created on: 03-Jan-2011
 *      
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "WordIndexDictionary.h"

//!A marker interface for the LDA graphical model
//!and its extensions.
/**
 * A model should be able to compute its contribution to
 * the log-likelihood, serialize to disk & also explain
 * itself by writing the word mixtures that represent the
 * latent topics to disk
 */
class Model {
public:
    const static int UNIGRAM = 1;

public:
    //!Model's contribution of log-likelihood
    virtual double get_eval() = 0;

    //!Serialize to disk
    virtual bool save() = 0;

    //!Explain: word mixtures for the latent topics
    virtual void write_statistics(WordIndexDictionary&) = 0;
};

#endif /* MODEL_H_ */
