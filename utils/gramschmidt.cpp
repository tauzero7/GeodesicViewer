// --------------------------------------------------------------------------------
/*
    gramschmidt.cpp

  Copyright (c) 2009-2015  Thomas Mueller, Frank Grave


   This file is part of the GeodesicViewer.

   The GeodesicViewer is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   The GeodesicViewer is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GeodesicViewer.  If not, see <http://www.gnu.org/licenses/>.
*/
// -------------------------------------------------------------------------------
#include "gramschmidt.h"

/*!
 *  All components are set to zero.
 */
GramSchmidt::GramSchmidt() {
    setVectors(m4d::vec4(), m4d::vec4(), m4d::vec4(), m4d::vec4());
}

GramSchmidt::GramSchmidt(const m4d::vec4 e0, const m4d::vec4 e1, const m4d::vec4 e2, const m4d::vec4 e3) {
    setVectors(e0, e1, e2, e3);
}

GramSchmidt::~GramSchmidt() {
}

// *********************************** public methods ******************************

void
GramSchmidt::setVectors(const m4d::vec4 e0, const m4d::vec4 e1, const m4d::vec4 e2, const m4d::vec4 e3) {
    e[0] = e0;
    e[1] = e1;
    e[2] = e2;
    e[3] = e3;
}

void
GramSchmidt::getVectors(m4d::vec4 &e0, m4d::vec4 &e1, m4d::vec4 &e2, m4d::vec4 &e3) {
    e0 = e[0];
    e1 = e[1];
    e2 = e[2];
    e3 = e[3];
}

bool
GramSchmidt::isOrthonormal() {
    double sc;
    bool orthonormal = false;

    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++) {
            sc = calcScalarProd(e[i], e[j]);
            if (fabs(sc) < DEF_GS_EPS) {
                orthonormal |= true;
            }
        }
    return orthonormal;
}

bool
GramSchmidt::isRightHanded() {
    gsl_matrix* m = gsl_matrix_alloc(4, 4);

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            gsl_matrix_set(m, i, j, e[i][j]);
        }

    int signum;
    gsl_permutation *p = gsl_permutation_alloc(4);
    gsl_linalg_LU_decomp(m, p, &signum);

    double det = gsl_linalg_LU_det(m, signum);

    gsl_permutation_free(p);
    gsl_matrix_free(m);

    return ((det > 0) ? true : false);
}


double
GramSchmidt::calcScalarProd(m4d::vec4 &v1, m4d::vec4 &v2) {
    double prod = 0.0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            prod += DEF_ETA(i, j) * v1.x(i) * v2.x(j);
            // std::cerr << i << " " << j << " " << DEF_ETA(i,j) << endl;
        }
    return prod;
}

bool
GramSchmidt::calculateTetrad() {
    m4d::vec4 u1 = e[1];
    m4d::vec4 u2 = e[2];
    m4d::vec4 u3 = e[3];

    e[1] = u1 + calcScalarProd(e[0], u1) * e[0];
    if (e[1].isZero()) {
        return false;
    }
    e[1].normalize();

    e[2] = u2 + calcScalarProd(e[0], u2) * e[0] - calcScalarProd(e[1], u2) * e[1];
    if (e[2].isZero()) {
        return false;
    }
    e[2].normalize();

    e[3] = u3 + calcScalarProd(e[0], u3) * e[0] - calcScalarProd(e[1], u3) * e[1] - calcScalarProd(e[2], u3) * e[2];
    if (e[3].isZero()) {
        return false;
    }
    e[3].normalize();

    return true;
}

bool
GramSchmidt::calculateTetrad(m4d::vec4 &e0, m4d::vec4 &e1, m4d::vec4 &e2, m4d::vec4 &e3) {
    setVectors(e0, e1, e2, e3);
    if (!calculateTetrad()) {
        return false;
    }
    getVectors(e0, e1, e2, e3);
    return true;
}

void
GramSchmidt::print(FILE*  fptr) {
    fprintf(fptr, "\nLocalTetrad\n");
    fprintf(fptr, " e0: %12.8f %12.8f %12.8f %12.8f\n", e[0][0], e[0][1], e[0][2], e[0][3]);
    fprintf(fptr, " e1: %12.8f %12.8f %12.8f %12.8f\n", e[1][0], e[1][1], e[1][2], e[1][3]);
    fprintf(fptr, " e2: %12.8f %12.8f %12.8f %12.8f\n", e[2][0], e[2][1], e[2][2], e[2][3]);
    fprintf(fptr, " e3: %12.8f %12.8f %12.8f %12.8f\n", e[3][0], e[3][1], e[3][2], e[3][3]);
}
