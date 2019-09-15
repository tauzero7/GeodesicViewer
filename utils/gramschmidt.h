/**
 * @file    gramschmidt.h
 * @author  Thomas Mueller
 *
 * @brief  Orthonormalize four base vectors.
 *
 * This file is part of GeodesicView.
 */
#ifndef GRAM_SCHMIDT_H
#define GRAM_SCHMIDT_H

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>

#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>

#define DEF_GS_EPS 1e-8

#define DEF_ETA(i, j) (i != j ? 0.0 : (i == 0 ? -1.0 : 1.0))

/**
 * @brief The GramSchmidt class
 */
class GramSchmidt {
public:
    GramSchmidt();
    GramSchmidt(const m4d::vec4 e0, const m4d::vec4 e1, const m4d::vec4 e2, const m4d::vec4 e3);
    ~GramSchmidt();

public:
    void setVectors(const m4d::vec4 e0, const m4d::vec4 e1, const m4d::vec4 e2, const m4d::vec4 e3);
    void getVectors(m4d::vec4& e0, m4d::vec4& e1, m4d::vec4& e2, m4d::vec4& e3);

    bool isOrthonormal();
    bool isRightHanded();

    double calcScalarProd(m4d::vec4& v1, m4d::vec4& v2);
    bool calculateTetrad();
    bool calculateTetrad(m4d::vec4& e0, m4d::vec4& e1, m4d::vec4& e2, m4d::vec4& e3);

    void print(FILE* fptr = stderr);

protected:
    m4d::vec4 e[4];
};

#endif // GRAM_SCHMIDT_H
