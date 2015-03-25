// --------------------------------------------------------------------------------
/*
    quaternions.h

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

/*!  \class  Quaternion
     \brief  Calculation with quaternions.

*/
// --------------------------------------------------------------------------------

#ifndef QUATERNIONS_H
#define QUATERNIONS_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>

#include <m4dGlobalDefs.h>

#define SIGNUM_SIGN(a)  ( a>=0.0 ? "+" : "-" )

#define QUATERNION_EPS  1.0e-15

// ---------------------------------------------------
//     class definition:   Quaternion
// ---------------------------------------------------
class Quaternion {
public:
    Quaternion();
    Quaternion(double x0, double x1, double x2, double x3);
    Quaternion(double x0, m4d::vec3 v);
    ~Quaternion();

// --------- public methods -----------
public:
    void      set(double x0, double x1, double x2, double x3);
    void      set(double x0, m4d::vec3 v);
    void      setReal(double x0);
    void      setI(double x1);
    void      setJ(double x2);
    void      setK(double x3);
    m4d::vec3 getVector();

    void      setRot(double angle,  double e1, double e2, double e3);
    void      setRot(double angle, m4d::vec3 v);

    double    r() const {
        return xr;
    }
    double    i() const {
        return xi;
    }
    double    j() const {
        return xj;
    }
    double    k() const {
        return xk;
    }

    Quaternion  conj() const;
    Quaternion  inv() const;
    double      length() const;

    void        operator= (const Quaternion &q);
    Quaternion  operator+ (const Quaternion &q) const;
    Quaternion  operator- (const Quaternion &q) const;

    Quaternion  operator* (const Quaternion &q) const;
    Quaternion  operator* (const double a) const;
    Quaternion  operator| (const Quaternion &rho) const;

    int         operator==(const Quaternion &q) const;
    int         operator!=(const Quaternion &q) const;

    void        print(FILE* ptr = stderr);
    void        printQ(FILE* ptr = stderr);


// --------- friend methods -----------
    friend Quaternion  operator*(double a, const Quaternion &q) {
        Quaternion n;
        n.set(a*q.r(), a*q.i(), a*q.j(), a*q.k());
        return n;
    }

// --------- protected attributes -----------
protected:
    double xr;
    double xi;
    double xj;
    double xk;
};

#endif
