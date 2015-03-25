// --------------------------------------------------------------------------------
/*
    gramschmidt.h

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

/*!  \class  GramSchmidt
     \brief  Orthonormalize four base vectors.

*/
// --------------------------------------------------------------------------------

#ifndef GRAM_SCHMIDT_H
#define GRAM_SCHMIDT_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>

#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>

#define DEF_GS_EPS 1e-8

#define DEF_ETA(i,j)  (i!=j ? 0.0 : (i==0 ? -1.0 : 1.0))

// ---------------------------------------------------
//     class definition:   GramSchmidt
// ---------------------------------------------------
class GramSchmidt {
public:
    GramSchmidt();
    GramSchmidt(const m4d::vec4 e0, const m4d::vec4 e1, const m4d::vec4 e2, const m4d::vec4 e3);
    ~GramSchmidt();

// --------- public methods -----------
public:
    void    setVectors(const m4d::vec4 e0, const m4d::vec4 e1, const m4d::vec4 e2, const m4d::vec4 e3);
    void    getVectors(m4d::vec4 &e0, m4d::vec4 &e1, m4d::vec4 &e2, m4d::vec4 &e3);

    bool    isOrthonormal();
    bool    isRightHanded();

    double  calcScalarProd(m4d::vec4 &v1, m4d::vec4 &v2);
    bool    calculateTetrad();
    bool    calculateTetrad(m4d::vec4 &e0, m4d::vec4 &e1, m4d::vec4 &e2, m4d::vec4 &e3);

    void  print(FILE*  fptr = stderr);

// --------- protected attributes -----------
protected:
    m4d::vec4     e[4];
};

#endif

