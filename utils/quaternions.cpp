/**
 * @file    quaternions.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "quaternions.h"

Quaternion::Quaternion()
{
    set(0.0, 0.0, 0.0, 0.0);
}

Quaternion::Quaternion(double x0, double x1, double x2, double x3)
{
    set(x0, x1, x2, x3);
}

Quaternion::Quaternion(double x0, m4d::vec3 v)
{
    set(x0, v[0], v[1], v[2]);
}

void Quaternion::calcMat()
{
    double len = this->length();
    double s = 1.0 / (len * len);

    mat.setElem(0, 0, 1.0 - 2.0 * s * (xj * xj + xk * xk));
    mat.setElem(0, 1, 2.0 * s * (xi * xj - xk * xr));
    mat.setElem(0, 2, 2.0 * s * (xi * xk + xj * xr));
    mat.setElem(0, 3, 0.0);

    mat.setElem(1, 0, 2.0 * s * (xi * xj + xk * xr));
    mat.setElem(1, 1, 1.0 - 2.0 * s * (xi * xi + xk * xk));
    mat.setElem(1, 2, 2.0 * s * (xj * xk - xi * xr));
    mat.setElem(1, 3, 0.0);

    mat.setElem(2, 0, 2.0 * s * (xi * xk - xj * xr));
    mat.setElem(2, 1, 2.0 * s * (xj * xk + xi * xr));
    mat.setElem(2, 2, 1.0 - 2.0 * s * (xi * xi + xj * xj));
    mat.setElem(2, 3, 0.0);

    mat.setElem(3, 0, 0.0);
    mat.setElem(3, 1, 0.0);
    mat.setElem(3, 2, 0.0);
    mat.setElem(3, 3, 1.0);
}

void Quaternion::set(double x0, double x1, double x2, double x3)
{
    xr = x0;
    xi = x1;
    xj = x2;
    xk = x3;
}

void Quaternion::set(double x0, m4d::vec3 v)
{
    xr = x0;
    xi = v[0];
    xj = v[1];
    xk = v[2];
}

void Quaternion::setReal(double x0)
{
    xr = x0;
}

void Quaternion::setI(double x1)
{
    xi = x1;
}

void Quaternion::setJ(double x2)
{
    xj = x2;
}

void Quaternion::setK(double x3)
{
    xk = x3;
}

m4d::vec3 Quaternion::getVector()
{
    return m4d::vec3(xi, xj, xk);
}

m4d::vec3 Quaternion::getCamDir()
{
    return m4d::vec3(mat[0][2], mat[1][2], mat[2][2]);
}

m4d::vec3 Quaternion::getCamRight()
{
    return m4d::vec3(mat[0][0], mat[1][0], mat[2][0]);
}

m4d::vec3 Quaternion::getCamVup()
{
    return m4d::vec3(mat[0][1], mat[1][1], mat[2][1]);
}

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

void Quaternion::setRot(double angle, m4d::vec3 v)
{
    m4d::vec3 vn = v.getNormalized();
    double sa = sin(0.5 * angle);
    xr = cos(0.5 * angle);
    xi = sa * vn[0];
    xj = sa * vn[1];
    xk = sa * vn[2];
}

Quaternion Quaternion::conj() const
{
    return Quaternion(xr, -xi, -xj, -xk);
}

Quaternion Quaternion::inv() const
{
    Quaternion n = conj();
    double val = length();

    if (fabs(val) < QUATERNION_EPS) {
        return Quaternion();
    }

    val = 1.0 / val;
    return n * val;
}

double Quaternion::length() const
{
    return sqrt(xr * xr + xi * xi + xj * xj + xk * xk);
}

void Quaternion::operator=(const Quaternion& q)
{
    xr = q.r();
    xi = q.i();
    xj = q.j();
    xk = q.k();
}

Quaternion Quaternion::operator+(const Quaternion& q) const
{
    Quaternion n;
    n.set(xr + q.r(), xi + q.i(), xj + q.j(), xk + q.k());
    return n;
}

Quaternion Quaternion::operator-(const Quaternion& q) const
{
    Quaternion n;
    n.set(xr - q.r(), xi - q.i(), xj - q.j(), xk - q.k());
    return n;
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
    Quaternion n;
    n.setReal(xr * q.r() - xi * q.i() - xj * q.j() - xk * q.k());
    n.setI(xr * q.i() + q.r() * xi + xj * q.k() - xk * q.j());
    n.setJ(xr * q.j() + q.r() * xj + xk * q.i() - xi * q.k());
    n.setK(xr * q.k() + q.r() * xk + xi * q.j() - xj * q.i());
    return n;
}

Quaternion Quaternion::operator*(const double a) const
{
    Quaternion n;
    n.set(xr * a, xi * a, xj * a, xk * a);
    return n;
}

Quaternion Quaternion::operator|(const Quaternion& rho) const
{
    Quaternion n;
    n = rho * (*this) * rho.conj();
    return n;
}

int Quaternion::operator==(const Quaternion& q) const
{
    return (fabs(xr - q.r()) <= QUATERNION_EPS && fabs(xi - q.i()) <= QUATERNION_EPS
        && fabs(xj - q.j()) <= QUATERNION_EPS && fabs(xk - q.k()) <= QUATERNION_EPS);
}

int Quaternion::operator!=(const Quaternion& q) const
{
    return !(*this == q);
}

void Quaternion::print(FILE* ptr)
{
    std::string si = SIGNUM_SIGN(xi);
    std::string sj = SIGNUM_SIGN(xj);
    std::string sk = SIGNUM_SIGN(xk);

    fprintf(ptr, "%8.5f %s%8.5fi %s%8.5fj %s%8.5fk\n", xr, si.c_str(), fabs(xi), sj.c_str(), fabs(xj), sk.c_str(),
        fabs(xk));
}

void Quaternion::printQ(FILE* ptr)
{
    fprintf(ptr, "(%f,%f,%f,%f)\n", xr, xi, xj, xk);
}
