/**
 * @file    openglJacobi_model.h
 * @author  Thomas Mueller
 *
 * @brief  Render engine for the Jacobi representation.
 *
 * This file is part of GeodesicView.
 */
#ifndef OPENGL_JACOBI_MODEL_H
#define OPENGL_JACOBI_MODEL_H

#include <QGLShaderProgram>
#include <QGLWidget>
#include <QMouseEvent>

#include <gdefs.h>
#include <utils/utilities.h>

#include <extra/m4dObject.h>
#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>

/**
 * @brief The OpenGLJacobiModel class
 */
class OpenGLJacobiModel : public QGLWidget {
    Q_OBJECT

public:
    OpenGLJacobiModel(struct_params* par, QWidget* parent = nullptr);
    virtual ~OpenGLJacobiModel();

public:
    void setActualJacobi(enum_jacobi_param jac);
    void showNumJacobi(int num);

    void reset();

protected:
    virtual void initializeGL();
    virtual void paintGL();
    void drawEllipse();

private:
    struct_params* mParams;
    enum_jacobi_param mActualJacobi;

    double mMajorAxis;
    double mMinorAxis;
    double mAngle;
    double mEllipticity;
    double mMu;

    int mShowJacobi;

    QColor mFGcolor;
    QColor mBGcolor;
};

#endif // OPENGL_JACOBI_MODEL_H
