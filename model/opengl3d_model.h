// --------------------------------------------------------------------------------
/*
    opengl3d_model.h

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

/*!  \class  OpenGL3dModel
     \brief  Render engine for the 3d representation of geodesics based on OpenGL.

*/
// --------------------------------------------------------------------------------

#ifndef OPENGL3D_MODEL_H
#define OPENGL3D_MODEL_H

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMouseEvent>

#include <gdefs.h>
#include <utils/camera.h>
#include <utils/myobject.h>
#include <utils/utilities.h>

#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>
#include <extra/m4dObject.h>


// ---------------------------------------------------
//    class definition:   OpenGL3dModel
// ---------------------------------------------------
class OpenGL3dModel : public QGLWidget {
    Q_OBJECT

public:
    OpenGL3dModel(struct_params* par, QWidget* parent = nullptr);
    virtual ~OpenGL3dModel();

// --------- public methods -----------
public:
    void  setPoints(m4d::enum_draw_type  dtype = m4d::enum_draw_pseudocart, bool update = true);
    void  clearPoints();

    void  setSachsAxes(bool update = true);
    void  clearSachsAxes();

    void  genEmbed(m4d::Metric* currMetric);
    void  clearEmbed();

    void  setMouseHandle(enum_mouse_handle  handle);

    void       setProjection(enum_projection    proj);
    void       setFieldOfView(double fov);
    double     getFieldOfView();

    void       setCameraPos(m4d::vec3 pos);
    m4d::vec3  getCameraPos();
    void       setCameraPoi(m4d::vec3 poi);
    m4d::vec3  getCameraPoi();
    void       setCameraVup(m4d::vec3 vup);
    m4d::vec3  getCameraVup();
    void       setCameraDirs(m4d::vec3 pos, m4d::vec3 poi, m4d::vec3 vup);
    void       getCameraDirs(m4d::vec3 &pos, m4d::vec3 &poi, m4d::vec3 &vup);

    void       setCameraSphere(double theta, double phi, double dist);
    void       getCameraSphere(double &theta, double &phi, double &dist);

    void  setCameraPredefs(enum_camera_predefs  type);

    void  clearAllObjects();
    void  insertObject(MyObject* obj);

    void  setFGcolor(QColor col);
    void  setBGcolor(QColor col);
    void  setEmbColor(QColor col);
    void  setColors(QColor fgcol, QColor bgcol, QColor embcol);

    void  setLineWidth(int width);
    void  setLineSmooth(int smooth);
    void  setStyle(enum_draw_style  style);

    void  setScaling(double sx, double sy, double sz);
    void  getScaling(double &sx, double &sy, double &sz);

    void  setStereoCam(bool useStereo);
    void  setStereoParams(double sep, enum_stereo_glasses type);
    void  setStereoType(enum_stereo_type type);

    void  setFog(bool useFog);
    void  setFogDensity(double density);

    void  playAnimRotation(bool play);
    void  setAnimRotationParams(double rx, double ry, double rz);
    void  getAnimRotationParams(double &rx, double &ry, double &rz);
    void  doAnimRotation(double msec, bool local = false);

    void  showNumVerts(int num);

    bool  saveRGBimage(QString filename);

    void  updateParams();
    void  reset();

// ------------ signals ---------------
signals:
    void  cameraMoved();

// -------- protected methods ---------
protected:
    virtual void  initializeGL();
    virtual void  paintGL();
    virtual void  paintGL_mono();
    virtual void  paintGL_stereo();
    virtual void  paintGL_axes();

    virtual void  keyPressEvent(QKeyEvent* event);
    virtual void  keyReleaseEvent(QKeyEvent* event);

    virtual void  mousePressEvent(QMouseEvent* event);
    virtual void  mouseReleaseEvent(QMouseEvent* event);
    virtual void  mouseMoveEvent(QMouseEvent* event);

    QString  getVertexShaderCode();
    QString  getFragmentShaderCode();


// -------- private attributes --------
private:
    struct_params*     mParams;
    Camera             mCamera;

    enum_mouse_handle  mMouseHandle;
    enum_projection    mProjection;
    enum_draw_style    mDrawStyle;
    bool               mStereo;

    QPoint             mLastPos;
    int                mKeyPressed;
    Qt::MouseButton    mButtonPressed;

    QColor             mFGcolor;
    QColor             mBGcolor;


    GLfloat*           mVerts;
    GLfloat*           mLambda;
    int                mNumVerts;
    int                mLineWidth;
    int                mLineSmooth;
    int                mShowNumVerts;

    GLfloat*           mSachsVerts1;
    int                mNumSachsVerts1;
    GLfloat*           mSachsVerts2;
    int                mNumSachsVerts2;
    enum_sachs_legs    mSachsLegs;
    float              mSachsScale;

    GLfloat*           mEmbVerts;
    int                mEmbNumVerts;
    unsigned int       mEmbNumElems;
    unsigned int       mEmbCounter;
    unsigned int**     mEmbIndices;
    GLsizei*           mCount;
    QColor             mEmbColor;

    std::vector<MyObject*>   mObjects;
    bool                     mWiredObjs;

    double    mScaleX;
    double    mScaleY;
    double    mScaleZ;

    GLfloat   light_diffuse[4];
    GLfloat   light_position[4];
    GLfloat   light_direction[3];

    bool      mUseFog;
    double    mFogDensity;

    bool      mPlayRot;
    double    mAnimRotX;
    double    mAnimRotY;
    double    mAnimRotZ;


    QString       mNameOfZaxis;
    GLUquadricObj *mQuadricAxis;
    GLUquadricObj *mQuadricDisk;
    float redLight[3];
    float greenLight[3];
    float blueLight[3];

    bool              mGLSLsupported;
    QGLShaderProgram* shader;
};

#endif
