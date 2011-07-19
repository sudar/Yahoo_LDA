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
 * Dirichlet.cpp
 *
 * Has code that computes the log_gamma & digamma functions
 * Parts of the code are related to the cc/mallet/types/Dirichlet.java
 * file in the Mallet code base
 *  Created on: 13 May, 2009
 *      
 */

#include <cmath>
#include "Dirichlet.h"

static const double NEG_EULER_MASCHERONI = -0.5772156649015328606065121;
static const double HALF_LOG_TWO_PI = log(2 * M_PI) / 2;

static const double DIGAMMA_COEFF_1 = 1 / 12;
static const double DIGAMMA_COEFF_2 = 1 / 120;
static const double DIGAMMA_COEFF_3 = 1 / 252;
static const double DIGAMMA_COEFF_4 = 1 / 240;
static const double DIGAMMA_COEFF_5 = 1 / 132;
static const double DIGAMMA_COEFF_6 = 691 / 32760;
static const double DIGAMMA_COEFF_7 = 1 / 12;

static const double DIGAMMA_LARGE = 9.5;
static const double DIGAMMA_SMALL = .000001;

double log_gamma(double z) {
    if (z < 0) {
        return 0;
    }

    int shift = (z<2) ? ceil(2-z) : 0;
    z += shift;

    double result = HALF_LOG_TWO_PI + ((z - 0.5) * log(z)) - z + (1 / (12 * z))
            - (1 / (360 * z * z * z)) + (1 / (1260 * z * z * z * z * z));

    while (shift-- > 0) {
        z--;
        result -= log(z);
    }

    return result;
}

double digamma(double z) {
    if (z < 0) {
        return 0;
    }
    double psi = 0;

    if (z < DIGAMMA_SMALL) {
        psi = NEG_EULER_MASCHERONI - (1 / z);
        return psi;
    }

    while (z < DIGAMMA_LARGE) {
        psi -= 1 / z;
        z++;
    }

    double inv_z = 1 / z;
    double inv_z_squared = inv_z * inv_z;

    psi += log(z) - .5 * inv_z - inv_z_squared * (DIGAMMA_COEFF_1 - inv_z_squared
            * (DIGAMMA_COEFF_2 - inv_z_squared * (DIGAMMA_COEFF_3 - inv_z_squared
                    * (DIGAMMA_COEFF_4 - inv_z_squared * (DIGAMMA_COEFF_5
                            - inv_z_squared * (DIGAMMA_COEFF_6 - inv_z_squared
                                    * DIGAMMA_COEFF_7))))));

    return psi;
}
