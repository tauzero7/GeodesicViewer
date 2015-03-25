// --------------------------------------------------------------------------------
/*
    openglJacobi_model.h

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

/*!  \class  OpenGLJacobiModel
     \brief  Render engine for the Jacobi representation.

*/
// --------------------------------------------------------------------------------

#ifndef OPENGL_JACOBI_MODEL_H
#define OPENGL_JACOBI_MODEL_H

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMouseEvent>

#include <gdefs.h>
#include <utils/utilities.h>

#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>
#include <extra/m4dObject.h>


// ---------------------------------------------------
//    class definition:   OpenGL3dModel
// ---------------------------------------------------
class OpenGLJacobiModel : public QGLWidget {
    Q_OBJECT

public:
    OpenGLJacobiModel(struct_params* par, QWidget* parent = 0);
    virtual ~OpenGLJacobiModel();

// --------- public methods -----------
public:
    void  setActualJacobi(enum_jacobi_param  jac);
    void  showNumJacobi(int num);

    void  reset();

// -------- protected methods ---------
protected:
    virtual void  initializeGL();
    virtual void  paintGL();
    void  drawEllipse();


// -------- private attributes --------
private:
    struct_params*     mParams;
    enum_jacobi_param  mActualJacobi;

    double             mMajorAxis;
    double             mMinorAxis;
    double             mAngle;
    double             mEllipticity;
    double             mMu;

    int                mShowJacobi;

    QColor             mFGcolor;
    QColor             mBGcolor;
};

#endif
