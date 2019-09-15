/**
 * @file    openglJacobi_model.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "openglJacobi_model.h"
#include <fstream>
#include <math/TransCoordinates.h>

#define DEF_EDLG 0.434294481903252 // 1.0/log(10.0)

extern m4d::Object mObject;

OpenGLJacobiModel::OpenGLJacobiModel(struct_params* par, QWidget* parent)
    : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::AlphaChannel), parent)
{
    mParams = par;

    setMinimumSize(200, 160);
    setMaximumSize(200, 160);

    mBGcolor = mParams->opengl_bg_color;
    mFGcolor = mParams->opengl_line_color;

    mActualJacobi = enum_jacobi_ellipse;

    mMajorAxis = mMinorAxis = 1.0;
    mAngle = 0.0;
    mEllipticity = 0.0;
    mMu = 1.0;
}

OpenGLJacobiModel::~OpenGLJacobiModel()
{
}

void OpenGLJacobiModel::setActualJacobi(enum_jacobi_param jac)
{
    mActualJacobi = jac;
    updateGL();
}

void OpenGLJacobiModel::showNumJacobi(int num)
{
    if (num > 0 && num <= static_cast<int>(mObject.jacobi.size())) {
        int n = num;
        if (num == static_cast<int>(mObject.jacobi.size())) {
            n--;
        }

        mMajorAxis = mObject.jacobi[n][0];
        mMinorAxis = mObject.jacobi[n][1];
        mMu = mObject.jacobi[n][2];
        mAngle = mObject.jacobi[n][3];
        mEllipticity = mObject.jacobi[n][4];
        mShowJacobi = num;
    } else {
        mMajorAxis = mMinorAxis = 1.0;
        mMu = 1.0;
        mAngle = 0.0;
        mEllipticity = 0.0;
        mShowJacobi = 0;
    }

    updateGL();
}

void OpenGLJacobiModel::reset()
{
    mMajorAxis = mMinorAxis = 1.0;
    mAngle = 0.0;
    mEllipticity = 0.0;
    mMu = 1.0;
    updateGL();
}

void OpenGLJacobiModel::initializeGL()
{
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.5, 1.5, -1.2, 1.2);
    glViewport(0, 0, width(), height());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OpenGLJacobiModel::paintGL()
{
    int i = 0;
    glClearColor(mBGcolor.redF(), mBGcolor.greenF(), mBGcolor.blueF(), 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1, 1, 0);
    if (mObject.type != m4d::enum_geodesic_lightlike_sachs) {
        return;
    }

    int curr = static_cast<int>(mActualJacobi);

#define DEF_USE_LAMBDA_NUMS 1

#if DEF_USE_LAMBDA_NUMS == 1
    double lambdaMax = static_cast<double>(mObject.lambda.size());
#else
    double lambdaMax = mObject.lambda[mObject.lambda.size() - 1];
#endif

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (mActualJacobi) {
    case enum_jacobi_ellipse:
        gluOrtho2D(-1.5, 1.5, -1.2, 1.2);
        glColor3f(0.4f, 0.4f, 0.4f);
        glBegin(GL_LINES);
        glVertex2f(-1.1f, 0.0f);
        glVertex2f(1.1f, 0.0f);
        glVertex2f(0.0f, -1.1f);
        glVertex2f(0.0f, 1.1f);

        glEnd();
        glColor3f(1.0f, 1.0f, 0.0f);
        drawEllipse();
        break;
    case enum_jacobi_dp:
    case enum_jacobi_dm:
    case enum_jacobi_ellipt:
        gluOrtho2D(0.0, lambdaMax, 0.0, mObject.maxJacobi[curr]);
        glColor3f(1, 1, 0);
        glBegin(GL_LINE_STRIP);
        while (i < static_cast<int>(mObject.jacobi.size()) && i < mShowJacobi) {
#if DEF_USE_LAMBDA_NUMS == 1
            glVertex2f((double)i, mObject.jacobi[i][curr]);
#else
            glVertex2f(mObject.lambda[i], mObject.jacobi[i][curr]);
#endif
            i++;
        }

        glEnd();
        break;
    case enum_jacobi_angle:
        gluOrtho2D(0.0, lambdaMax, -M_PI, M_PI);
        glColor3f(0.4f, 0.4f, 0.4f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(static_cast<float>(lambdaMax), 0.0f);

        glEnd();
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_LINE_STRIP);
        while (i < static_cast<int>(mObject.jacobi.size()) && i < mShowJacobi) {
#if DEF_USE_LAMBDA_NUMS == 1
            glVertex2f(static_cast<double>(i), static_cast<float>(mObject.jacobi[i][curr]));
#else
            glVertex2f(mObject.lambda[i], mObject.jacobi[i][curr]);
#endif
            i++;
        }

        glEnd();
        break;
    case enum_jacobi_mu:
        gluOrtho2D(0.0, lambdaMax, -4.0, log(mObject.maxJacobi[curr]) * DEF_EDLG);
        glColor3f(0.4f, 0.4f, 0.4f);
        glBegin(GL_LINES);

        glVertex2f(0.0f, 0.0f);
        glVertex2f(static_cast<float>(lambdaMax), 0.0f);

        glEnd();
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_LINE_STRIP);
        while (i < (int)mObject.jacobi.size() && i < mShowJacobi)

        {
#if DEF_USE_LAMBDA_NUMS == 1
            glVertex2f(static_cast<float>(i), log(mObject.jacobi[i][curr]) * DEF_EDLG);
#else
            glVertex2f(mObject.lambda[i], log(mObject.jacobi[i][curr]) * DEF_EDLG);
#endif
            i++;
        }
        glEnd();
        break;
    }
}

void OpenGLJacobiModel::drawEllipse()
{
    float a = 1.0f;
    float b = float(mMinorAxis / mMajorAxis);
    float psi = static_cast<float>(mAngle);

    glBegin(GL_LINE_LOOP);
    for (float phi = 0.0f; phi < 2.0f * M_PI; phi += 0.01f) {
        glVertex2f(a * cosf(phi) * cosf(psi) - b * sinf(phi) * sinf(psi), a * cosf(phi) * sinf(psi) + b * sinf(phi) * cosf(psi));
    }
    glEnd();
}
