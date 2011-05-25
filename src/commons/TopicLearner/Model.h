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
