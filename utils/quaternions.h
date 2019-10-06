/**
 * @file    quaternions.h
 * @author  Thomas Mueller
 *
 * @brief  Calculation with quaternions.
 *
 * This file is part of GeodesicView.
 */
#ifndef QUATERNIONS_H
#define QUATERNIONS_H

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>

#include <m4dGlobalDefs.h>

#define SIGNUM_SIGN(a) (a >= 0.0 ? "+" : "-")

#define QUATERNION_EPS 1.0e-15

/**
 * @brief The Quaternion class
 */
class Quaternion
{
public:
    Quaternion();
    Quaternion(double x0, double x1, double x2, double x3);
    Quaternion(double x0, m4d::vec3 v);

    void calcMat();

    /*! Set all parameters of the Quaternion.
     *  @param x0 : real component
     *  @param x1 : i-component
     *  @param x2 : j-component
     *  @param x3 : k-component
     */
    void set(double x0, double x1, double x2, double x3);

    /*! Set all parameters of the Quaternion.
     *  @param x0 : real component
     *  @param v  : vector component
     */
    void set(double x0, m4d::vec3 v);
    void setReal(double x0);
    void setI(double x1);
    void setJ(double x2);
    void setK(double x3);

    /*! Get vector part of quaternion.
     *  @return vec3 : vector component
     */
    m4d::vec3 getVector();

    m4d::vec3 getCamDir();
    m4d::vec3 getCamRight();
    m4d::vec3 getCamVup();

    /*! Set rotation quaternion.
     *   (e1,e2,e3) build up the rotation axis which will be normalized.
     *   The resulting Quaternion is given by  cos(angle/2) + sin(angle/2)*(e1*i + e2*j + e3*k).
     *   If (e1=e2=e3=0.0) the Quaternion will not be set!
     *
     *   \param  angle :  angle of the rotation in radians
     *   \param  e1    :  i-component
     *   \param  e2    :  j-component
     *   \param  e3    :  k-component
     */
    void setRot(double angle, double e1, double e2, double e3);

    /*! Set rotation quaternion.
     *   v represents the rotation axis which will be normalized.
     *   The resulting Quaternion is given by  cos(angle/2) + sin(angle/2)*(e1*i + e2*j + e3*k).
     *   If v=vec3() the Quaternion will not be set!
     *
     *   \param  angle :  angle of the rotation in radians
     *   \param  v     :  vector component.
     */
    void setRot(double angle, m4d::vec3 v);

    double r() const { return xr; }
    double i() const { return xi; }
    double j() const { return xj; }
    double k() const { return xk; }

    /*! Calculate and return conjugate.
     *  \return Quaternion : conj(x) = x0 - x1*i - x2*j - x3*k
     */
    Quaternion conj() const;

    /*! Calculate and return inverse.
     *  \return Quaternion : x^-1 = conj(x)/abs(x)^2
     */
    Quaternion inv() const;

    /*! Calculate and return length.
     *  \return Length of the quaternion defined by |x| = sqrt(x*conj(x)).
     */
    double length() const;

    void operator=(const Quaternion& q);

    /*! Addition.
     *  The addition of two quaternions x and y is defined by
     *  z = ( x.r + y.r, x.i + y.i, x.j + y.j, x.k + y.k )
     */
    Quaternion operator+(const Quaternion& q) const;

    /*! Subtraction.
     *  The subtraction of quaternion y from x is defined by
     *  z = ( x.r - y.r, x.i - y.i, x.j - y.j, x.k - y.k )
     * \param q : Quaternion y.
     */
    Quaternion operator-(const Quaternion& q) const;

    /*! Multiplication of two Quaternions.
     *  The multiplication between two quaternions x and y is defined by:
     *  z.r = x.r*y.r - x.i*y.i - x.j*y.j - x.k-y.k
     *  z.i = x.r*y.i + y.r*x.i + x.j*y.k - x.k-y.j
     *  z.j = x.r*y.j + y.r*x.j + x.k*y.i - x.i-y.k
     *  z.k = x.r*y.k + y.r*x.k + x.i*y.j - x.j-y.i
     *  \param q : Quaternion y.
     */
    Quaternion operator*(const Quaternion& q) const;

    /*! Scalar multiplication.
     *  \param a : scalar.
     *  \return Quaternion : scalar multiplied quaternion.
     */
    Quaternion operator*(const double a) const;

    /*! Operator|.
     *  \param   rho : rotation Quaternion must be set by setRot().
     *  \return  Quaterion :  x|rho  with  rho:  x -> rho*x*conj(rho).
     */
    Quaternion operator|(const Quaternion& rho) const;

    /*! Logical operator "equal".
     *  \param  q : reference to Quaternion.
     */
    int operator==(const Quaternion& q) const;

    /*!  Logical operator "not-equal".
     *  \param  q : reference to Quaternion.
     */
    int operator!=(const Quaternion& q) const;

    /*! Print Quaternion to ptr.
     *  Standard printout, e.g.:  3.1 + 2.5i + 1.2j + 4.6j.
     *  \param  ptr :  file pointer.
     */
    void print(FILE* ptr = stderr);

    /*! Print Quaternion to ptr.
     * Standard printout in vector notation, e.g. (3.1,2.5,1.2,4.6).
     */
    void printQ(FILE* ptr = stderr);

    friend Quaternion operator*(double a, const Quaternion& q)
    {
        Quaternion n;
        n.set(a * q.r(), a * q.i(), a * q.j(), a * q.k());
        return n;
    }

protected:
    double xr;
    double xi;
    double xj;
    double xk;
    m4d::mat4 mat;
};

#endif
