/**
 * @file    quaternions.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "quaternions.h"

/*! Create a new Quaternion.
 *  All components are set to zero.
 */
Quaternion::Quaternion()
{
    set(0.0, 0.0, 0.0, 0.0);
}

/*! Create a new Quaternion.
 *  \param x0 : real component
 *  \param x1 : i-component
 *  \param x2 : j-component
 *  \param x3 : k-component
 */
Quaternion::Quaternion(double x0, double x1, double x2, double x3)
{
    set(x0, x1, x2, x3);
}

Quaternion::Quaternion(double x0, m4d::vec3 v)
{
    set(x0, v[0], v[1], v[2]);
}

Quaternion::~Quaternion()
{
}

/*! Set all parameters of the Quaternion.
 *  \param x0 : real component
 *  \param x1 : i-component
 *  \param x2 : j-component
 *  \param x3 : k-component
 */
void Quaternion::set(double x0, double x1, double x2, double x3)
{
    xr = x0;
    xi = x1;
    xj = x2;
    xk = x3;
}

/*! Set all parameters of the Quaternion.
 *  \param x0 : real component
 *  \param v  : vector component
 */
void Quaternion::set(double x0, m4d::vec3 v)
{
    xr = x0;
    xi = v[0];
    xj = v[1];
    xk = v[2];
}

/*! Set real component.
 *  \param x0 : real component
 */
void Quaternion::setReal(double x0)
{
    xr = x0;
}

/*! Set i-component.
 *  \param x1 : i-component
 */
void Quaternion::setI(double x1)
{
    xi = x1;
}

/*! Set j-component.
 *  \param x2 : j-component
 */
void Quaternion::setJ(double x2)
{
    xj = x2;
}

/*! Set k-component.
 *  \param x3 : k-component
 */
void Quaternion::setK(double x3)
{
    xk = x3;
}

/*! Get vector part of quaternion.
 *  \return vec3 : vector component
 */
m4d::vec3
Quaternion::getVector()
{
    return m4d::vec3(xi, xj, xk);
}

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
void Quaternion::setRot(double angle, double e1, double e2, double e3)
{
    double norm = sqrt(e1 * e1 + e2 * e2 + e3 * e3);
    if (norm <= 0.0) {
        return;
    }

    norm = 1.0 / norm;

    double sa = sin(0.5 * angle);
    xr = cos(0.5 * angle);
    xi = sa * e1 * norm;
    xj = sa * e2 * norm;
    xk = sa * e3 * norm;
}

/*! Set rotation quaternion.
 *   v represents the rotation axis which will be normalized.
 *   The resulting Quaternion is given by  cos(angle/2) + sin(angle/2)*(e1*i + e2*j + e3*k).
 *   If v=vec3() the Quaternion will not be set!
 *
 *   \param  angle :  angle of the rotation in radians
 *   \param  v     :  vector component.
 */
void Quaternion::setRot(double angle, m4d::vec3 v)
{
    m4d::vec3 vn = v.getNormalized();
    double sa = sin(0.5 * angle);
    xr = cos(0.5 * angle);
    xi = sa * v[0];
    xj = sa * v[1];
    xk = sa * v[2];
}

/*! Calculate and return conjugate.
 *  \return Quaternion : conj(x) = x0 - x1*i - x2*j - x3*k
 */
Quaternion
Quaternion::conj() const
{
    return Quaternion(xr, -xi, -xj, -xk);
}

/*! Calculate and return inverse.
 *  \return Quaternion : x^-1 = conj(x)/abs(x)^2
 */
Quaternion
Quaternion::inv() const
{
    Quaternion n = conj();
    double val = length();

    if (fabs(val) < QUATERNION_EPS) {
        return Quaternion();
    }

    val = 1.0 / val;
    return n * val;
}

/*! Calculate and return length.
 *  \return Length of the quaternion defined by |x| = sqrt(x*conj(x)).
 */
double
Quaternion::length() const
{
    return sqrt(xr * xr + xi * xi + xj * xj + xk * xk);
}

/*! Assignment operator.
 *  \param q : reference two quaternion.
 */
void Quaternion::operator=(const Quaternion& q)
{
    xr = q.r();
    xi = q.i();
    xj = q.j();
    xk = q.k();
}

/*! Addition.
 *  The addition of two quaternions x and y is defined by
 *  z = ( x.r + y.r, x.i + y.i, x.j + y.j, x.k + y.k )
 */
Quaternion
Quaternion::operator+(const Quaternion& q) const
{
    Quaternion n;
    n.set(xr + q.r(), xi + q.i(), xj + q.j(), xk + q.k());
    return n;
}

/*! Subtraction.
 *  The subtraction of quaternion y from x is defined by
 *  z = ( x.r - y.r, x.i - y.i, x.j - y.j, x.k - y.k )
 * \param q : Quaternion y.
 */
Quaternion
Quaternion::operator-(const Quaternion& q) const
{
    Quaternion n;
    n.set(xr - q.r(), xi - q.i(), xj - q.j(), xk - q.k());
    return n;
}

/*! Multiplication of two Quaternions.
 *  The multiplication between two quaternions x and y is defined by:
 *  z.r = x.r*y.r - x.i*y.i - x.j*y.j - x.k-y.k
 *  z.i = x.r*y.i + y.r*x.i + x.j*y.k - x.k-y.j
 *  z.j = x.r*y.j + y.r*x.j + x.k*y.i - x.i-y.k
 *  z.k = x.r*y.k + y.r*x.k + x.i*y.j - x.j-y.i
 *  \param q : Quaternion y.
 */
Quaternion
    Quaternion::operator*(const Quaternion& q) const
{
    Quaternion n;
    n.setReal(xr * q.r() - xi * q.i() - xj * q.j() - xk * q.k());
    n.setI(xr * q.i() + q.r() * xi + xj * q.k() - xk * q.j());
    n.setJ(xr * q.j() + q.r() * xj + xk * q.i() - xi * q.k());
    n.setK(xr * q.k() + q.r() * xk + xi * q.j() - xj * q.i());
    return n;
}

/*! Scalar multiplication.
 *  \param a : scalar.
 *  \return Quaternion : scalar multiplied quaternion.
 */
Quaternion
    Quaternion::operator*(const double a) const
{
    Quaternion n;
    n.set(xr * a, xi * a, xj * a, xk * a);
    return n;
}

/*! Operator|.
 *  \param   rho : rotation Quaternion must be set by setRot().
 *  \return  Quaterion :  x|rho  with  rho:  x -> rho*x*conj(rho).
 */
Quaternion
Quaternion::operator|(const Quaternion& rho) const
{
    Quaternion n;
    n = rho * (*this) * rho.conj();
    return n;
}

/*! Logical operator "equal".
 *  \param  q : reference to Quaternion.
 */
int Quaternion::operator==(const Quaternion& q) const
{
    return (fabs(xr - q.r()) <= QUATERNION_EPS && fabs(xi - q.i()) <= QUATERNION_EPS && fabs(xj - q.j()) <= QUATERNION_EPS && fabs(xk - q.k()) <= QUATERNION_EPS);
}

/*!  Logical operator "not-equal".
 *  \param  q : reference to Quaternion.
 */
int Quaternion::operator!=(const Quaternion& q) const
{
    return !(*this == q);
}

/*! Print Quaternion to ptr.
 *  Standard printout, e.g.:  3.1 + 2.5i + 1.2j + 4.6j.
 *  \param  ptr :  file pointer.
 */
void Quaternion::print(FILE* ptr)
{
    std::string si = SIGNUM_SIGN(xi);
    std::string sj = SIGNUM_SIGN(xj);
    std::string sk = SIGNUM_SIGN(xk);

    fprintf(ptr, "%8.5f %s%8.5fi %s%8.5fj %s%8.5fk\n", xr, si.c_str(), fabs(xi), sj.c_str(), fabs(xj), sk.c_str(), fabs(xk));
}

/*! Print Quaternion to ptr.
 * Standard printout in vector notation, e.g. (3.1,2.5,1.2,4.6).
 */
void Quaternion::printQ(FILE* ptr)
{
    fprintf(ptr, "(%f,%f,%f,%f)\n", xr, xi, xj, xk);
}
