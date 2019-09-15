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
class Quaternion {
public:
    Quaternion();
    Quaternion(double x0, double x1, double x2, double x3);
    Quaternion(double x0, m4d::vec3 v);
    ~Quaternion();

    // --------- public methods -----------
public:
    void set(double x0, double x1, double x2, double x3);
    void set(double x0, m4d::vec3 v);
    void setReal(double x0);
    void setI(double x1);
    void setJ(double x2);
    void setK(double x3);
    m4d::vec3 getVector();

    void setRot(double angle, double e1, double e2, double e3);
    void setRot(double angle, m4d::vec3 v);

    double r() const
    {
        return xr;
    }
    double i() const
    {
        return xi;
    }
    double j() const
    {
        return xj;
    }
    double k() const
    {
        return xk;
    }

    Quaternion conj() const;
    Quaternion inv() const;
    double length() const;

    void operator=(const Quaternion& q);
    Quaternion operator+(const Quaternion& q) const;
    Quaternion operator-(const Quaternion& q) const;

    Quaternion operator*(const Quaternion& q) const;
    Quaternion operator*(const double a) const;
    Quaternion operator|(const Quaternion& rho) const;

    int operator==(const Quaternion& q) const;
    int operator!=(const Quaternion& q) const;

    void print(FILE* ptr = stderr);
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
};

#endif
