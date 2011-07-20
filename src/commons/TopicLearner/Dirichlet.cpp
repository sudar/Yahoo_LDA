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
