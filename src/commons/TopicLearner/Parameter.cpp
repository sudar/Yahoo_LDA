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
 * Paramete.cpp
 *
 *  Created on: 14-May-2010
 *      
 */

#include "Parameter.h"
#include "DocumentReader.h"
#include "DocumentWriter.h"
#include "document.pb.h"

Parameter::Parameter() {
    length = 0;
    sum = 0;
    values = NULL;
}

Parameter::~Parameter() {
    if (values != NULL)
        delete[] values;
}

Parameter::Parameter(const Parameter &param) {
    length = 0;
    sum = 0;
    values = NULL;
    *this = param;
}

void Parameter::initialize_from_values(int length_, const double* values_,
        float sum_) {
    length = length_;
    sum = sum_;
    values = new double[length];
    if (values_ != NULL) {
        memcpy(values, values_, sizeof(double) * length_);
    } else {
        for (int i = 0; i < length; i++)
            values[i] = sum / length;
    }
}

void Parameter::initialize_from_dump(string fname) {
    using namespace LDA;
    parameters* params = new parameters;
    DocumentReader doc_rdr(fname);
    doc_rdr.read(params);
    if (params->has_alphasum()) {
        length = params->alphas_size();
        values = new double[length];
        for (int i = 0; i < length; i++)
            values[i] = params->alphas(i);
        sum = params->alphasum();
    }
    delete params;
}

void Parameter::dump(string fname) {
    DocumentWriter doc_writer(fname);
    parameters params;
    params.set_alphasum(sum);
    for (int i = 0; i < length; i++)
        params.add_alphas(values[i]);
    doc_writer.write(params);
}

Parameter& Parameter::operator=(const Parameter &param) {
    length = param.length;
    sum = param.sum;
    if (values != NULL) {
        delete[] values;
        values = NULL;
    }

    if (length > 0) {
        values = new double[length];
        memcpy(values, param.values, sizeof(double) * length);
    }
    return *this;
}
