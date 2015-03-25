// --------------------------------------------------------------------------------
/*
    opengl3d_model.cpp

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
// -------------------------------------------------------------------------------

#include <fstream>
#include "opengl3d_model.h"
#include <math/TransCoordinates.h>

extern m4d::Object mObject;

/*! Standard constructor.
 *  \param par : pointer to struct.
 *  \param parent : pointer to parent widget.
 */
OpenGL3dModel::OpenGL3dModel(struct_params* par, QWidget* parent)
    : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::AlphaChannel), parent) {
    mParams = par;

    mCamera.setSizeAndAspect(DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT);
    mCamera.setEyePos(m4d::vec3(&mParams->opengl_eye_pos[0]));
    mCamera.setPOI(m4d::vec3(&mParams->opengl_eye_poi[0]));
    mCamera.setVup(m4d::vec3(&mParams->opengl_eye_vup[0]));
    mCamera.setFovY(mParams->opengl_fov);

    mButtonPressed = Qt::NoButton;
    setFocusPolicy(Qt::ClickFocus);
    mKeyPressed = Qt::Key_No;

    setMinimumSize(DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT);
    setMaximumSize(DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT);

    mBGcolor = mParams->opengl_bg_color;
    mFGcolor = mParams->opengl_line_color;

    mEmbColor  = Qt::lightGray;
    mLineWidth = mParams->opengl_line_width;

    mMouseHandle = enum_mouse_rotate;
    mProjection  = (enum_projection)mParams->opengl_projection;
    mDrawStyle   = enum_draw_lines;

    // Vertices for the geodesic.
    mVerts        = NULL;
    mLambda       = NULL;
    mNumVerts     = 0;
    mShowNumVerts = 0;

    mSachsLegs      = mParams->opengl_sachs_legs;
    mSachsScale     = mParams->opengl_sachs_scale;
    mSachsVerts1    = NULL;
    mSachsVerts2    = NULL;
    mNumSachsVerts1 = 0;
    mNumSachsVerts2 = 0;

    // Data structures for an embedding diagram.
    mEmbVerts    = NULL;
    mEmbNumVerts = 0;
    mEmbIndices  = NULL;
    mEmbCounter  = 0;
    mCount       = NULL;


    mScaleX = mScaleY = mScaleZ = 1.0;

    mStereo = (bool)mParams->opengl_stereo_use;
    mUseFog = (bool)mParams->opengl_fog_use;
    mFogDensity = mParams->opengl_fog_init; //DEF_OPENGL_FOG_DENSITY_INIT;
    QGLFormat f = format();

    mPlayRot = true;
    mAnimRotX = DEF_OPENGL_ANIM_ROT_X_INIT;
    mAnimRotY = DEF_OPENGL_ANIM_ROT_Y_INIT;
    mAnimRotZ = DEF_OPENGL_ANIM_ROT_Z_INIT;


    if (!mObjects.empty()) {
        mObjects.clear();
    }
    mWiredObjs = false;

    mQuadricAxis = gluNewQuadric();
    gluQuadricNormals(mQuadricAxis, GLU_SMOOTH);
    gluQuadricDrawStyle(mQuadricAxis, GLU_FILL);
    gluQuadricTexture(mQuadricAxis, GL_FALSE);
    mQuadricDisk = gluNewQuadric();
    gluQuadricNormals(mQuadricDisk, GLU_SMOOTH);
    gluQuadricDrawStyle(mQuadricDisk, GLU_FILL);
    gluQuadricTexture(mQuadricDisk, GL_FALSE);
    redLight[1] = redLight[2] = greenLight[0] = greenLight[2] = blueLight[0] = blueLight[1] = 0.0f;
    redLight[0] = greenLight[1] = blueLight[2] = 1.0f;

    mNameOfZaxis = QString("z");
}

/*! Standard destructor.
 */
OpenGL3dModel::~OpenGL3dModel() {
    if (mQuadricAxis != NULL) {
        gluDeleteQuadric(mQuadricAxis);
    }
    mQuadricAxis = NULL;
    if (mQuadricDisk != NULL) {
        gluDeleteQuadric(mQuadricDisk);
    }
    mQuadricDisk = NULL;
}

// ************************************ public methods ********************************
/*! Set geodesic points.
 *  \param  data : pointer to object.
 *  \param  dtype : type of drawing.
 *  \param  update : update gl.
 */
void
OpenGL3dModel::setPoints(m4d::enum_draw_type  dtype, bool update) {
    if (mVerts != NULL) {
        delete [] mVerts;
    }
    if (mLambda != NULL) {
        delete [] mLambda;
    }

    mNumVerts = mObject.points.size();
    mVerts    = new GLfloat[ mNumVerts * 3 ];
    mLambda   = new GLfloat[ mNumVerts ];

    GLfloat* vptr = mVerts;
    GLfloat* lptr = mLambda;

    mNameOfZaxis = QString("z");

    m4d::vec4 tp;
    for (int i = 0; i < mNumVerts; i++) {
        switch (dtype) {
            case m4d::enum_draw_pseudocart:
                mObject.currMetric->transToPseudoCart(mObject.points[i], tp);
                break;
            case m4d::enum_draw_coordinates:
                break;
            case m4d::enum_draw_embedding:
                mObject.currMetric->transToEmbedding(mObject.points[i], tp);
                break;
            case m4d::enum_draw_twoplusone:
                mObject.currMetric->transToTwoPlusOne(mObject.points[i], tp);
                mNameOfZaxis = QString("t");
                break;
            case m4d::enum_draw_effpoti:
                break;
            case m4d::enum_draw_custom:
                mObject.currMetric->transToCustom(mObject.points[i], tp);
                break;
        }
        *(vptr++) = GLfloat(tp[1]);
        *(vptr++) = GLfloat(tp[2]);
        *(vptr++) = GLfloat(tp[3]);

        *(lptr++) = GLfloat(mObject.lambda[i]);
    }

    mShowNumVerts = mNumVerts;
    if (update) {
        updateGL();
    }
}

/*! Set Sachs axes.
 *  \param data: pointer to object class.
 *  \param update: update OpenGL window.
 */
void OpenGL3dModel::setSachsAxes(bool update) {
    if (mSachsVerts1 != NULL) {
        delete [] mSachsVerts1;
    }
    if (mSachsVerts2 != NULL) {
        delete [] mSachsVerts2;
    }

    mNumSachsVerts1 = mObject.sachs1.size();
    mNumSachsVerts2 = mObject.sachs2.size();
    int mv = mObject.points.size();

    mSachsVerts1 = new GLfloat[ mNumSachsVerts1 * 3 * 2 ];
    mSachsVerts2 = new GLfloat[ mNumSachsVerts2 * 3 * 2 ];

    GLfloat* vptr1 = mSachsVerts1;
    GLfloat* vptr2 = mSachsVerts2;

    m4d::vec4 tp;
    m4d::vec4 d1, d2, s1, s2;

    double f, l1a, l1b, l2a, l2b;
    int n1 = 0, n2 = 0;
    while (n1 < mNumSachsVerts1 && n2 < mNumSachsVerts2 && n1 < mv) {
        // Umparametrisierung
        f = mObject.sachs1[n1].x(0) / mObject.dirs[n1].x(0);
        s1 = mObject.sachs1[n1] - f * mObject.dirs[n1];
        f = mObject.sachs2[n2].x(0) / mObject.dirs[n2].x(0);
        s2 = mObject.sachs2[n2] - f * mObject.dirs[n2];

        m4d::TransCoordinates::toCartesianCoord(mObject.currMetric->getCoordType(), mObject.points[n1], s1, tp, d1);
        m4d::TransCoordinates::toCartesianCoord(mObject.currMetric->getCoordType(), mObject.points[n1], s2, tp, d2);

        d1 *= mParams->opengl_sachs_scale;
        d2 *= mParams->opengl_sachs_scale;

        switch (mParams->opengl_sachs_legs) {
            default:
            case enum_sachs_legs_right_up:
                l1a = 0.0;
                l1b = 1.0;
                l2a = 0.0;
                l2b = 1.0;
                break;
            case enum_sachs_legs_up_left:
                l1a = 1.0;
                l1b = 0.0;
                l2a = 0.0;
                l2b = 1.0;
                break;
            case enum_sachs_legs_left_down:
                l1a = 1.0;
                l1b = 0.0;
                l2a = 1.0;
                l2b = 0.0;
                break;
            case enum_sachs_legs_down_right:
                l1a = 0.0;
                l1b = 1.0;
                l2a = 1.0;
                l2b = 0.0;
                break;
            case enum_sachs_legs_all:
                l1a = 1.0;
                l1b = 1.0;
                l2a = 1.0;
                l2b = 1.0;
                break;
        }

        *(vptr1++) = GLfloat(tp[1] - l1a * d1[1]);
        *(vptr1++) = GLfloat(tp[2] - l1a * d1[2]);
        *(vptr1++) = GLfloat(tp[3] - l1a * d1[3]);
        *(vptr1++) = GLfloat(tp[1] + l1b * d1[1]);
        *(vptr1++) = GLfloat(tp[2] + l1b * d1[2]);
        *(vptr1++) = GLfloat(tp[3] + l1b * d1[3]);

        *(vptr2++) = GLfloat(tp[1] - l2a * d2[1]);
        *(vptr2++) = GLfloat(tp[2] - l2a * d2[2]);
        *(vptr2++) = GLfloat(tp[3] - l2a * d2[3]);
        *(vptr2++) = GLfloat(tp[1] + l2b * d2[1]);
        *(vptr2++) = GLfloat(tp[2] + l2b * d2[2]);
        *(vptr2++) = GLfloat(tp[3] + l2b * d2[3]);
        n1++;
        n2++;
    }

    if (update) {
        updateGL();
    }
}

void
OpenGL3dModel::clearSachsAxes() {
    if (mSachsVerts1 != NULL) {
        delete [] mSachsVerts1;
    }
    if (mSachsVerts2 != NULL) {
        delete [] mSachsVerts2;
    }

    mSachsVerts1 = mSachsVerts2 = NULL;
    mNumSachsVerts1 = mNumSachsVerts2 = 0;
    updateGL();
}

/*! Delete points.
 */
void
OpenGL3dModel::clearPoints() {
    if (mVerts != NULL) {
        delete [] mVerts;
    }
    mVerts = NULL;

    if (mLambda != NULL) {
        delete [] mLambda;
    }
    mLambda = NULL;

    mNumVerts = 0;
    mShowNumVerts = 0;
    updateGL();
}


/*! Generate embedding vertices.
 *  \param  currMetric : pointer to current metric.
 */
void
OpenGL3dModel::genEmbed(m4d::Metric* currMetric) {
    if (mEmbVerts != NULL) {
        delete [] mEmbVerts;
    }
    mEmbVerts = NULL;

    if (mEmbIndices != NULL) {
        for (unsigned int j = 0; j < mEmbCounter; j++) {
            delete [] mEmbIndices[j];
        }
        delete [] mEmbIndices;
    }
    mEmbIndices = NULL;
    mEmbNumElems = mEmbCounter = 0;

    if (mCount != NULL) {
        delete [] mCount;
    }
    mCount = NULL;

    std::vector<m4d::vec3> vertices;
    std::vector<int>       indices;

    if (currMetric == NULL) {
        return;
    }

    if (currMetric->getEmbeddingVertices(vertices, indices, mEmbNumElems, mEmbCounter) == 0) {
        return;
    }

    mEmbNumVerts = vertices.size();

    mCount = new GLsizei[mEmbCounter];
    mEmbIndices = new unsigned int*[mEmbCounter];
    for (unsigned int i = 0; i < mEmbCounter; i++) {
        mEmbIndices[i] = new unsigned int[mEmbNumElems * 2];
    }

    //std::cerr << mEmbNumElems << " " << mEmbCounter << std::endl;

    unsigned int k = 0;
    for (unsigned int i = 0; i < mEmbCounter; i++) {
        for (unsigned int j = 0; j < mEmbNumElems * 2; j++) {
            mEmbIndices[i][j] = indices[k++];
        }
        mCount[i] = mEmbNumElems * 2;
    }


    mEmbVerts = new GLfloat[ mEmbNumVerts * 3 ];
    GLfloat* vptr = mEmbVerts;

    for (int i = 0; i < mEmbNumVerts; i++) {
        *(vptr++) = GLfloat(vertices[i][0]);
        *(vptr++) = GLfloat(vertices[i][1]);
        *(vptr++) = GLfloat(vertices[i][2]);
    }
    updateGL();
}

/*! Clear embedding vertices.
 */
void
OpenGL3dModel::clearEmbed() {
    if (mEmbVerts != NULL) {
        delete [] mEmbVerts;
    }
    mEmbVerts = NULL;
    mEmbNumVerts = 0;

    if (mEmbIndices != NULL) {
        for (unsigned int j = 0; j < mEmbCounter; j++) {
            delete [] mEmbIndices[j];
        }
        delete [] mEmbIndices;
    }
    mEmbIndices = NULL;

    if (mCount != NULL) {
        delete [] mCount;
    }
    mCount = NULL;

    mEmbNumElems = mEmbCounter = 0;
    updateGL();
}

/*! Set mounse handle.
 *  \param  handle : mouse handle index.
 *  \sa enum_mouse_handle.
 */
void
OpenGL3dModel::setMouseHandle(enum_mouse_handle  handle) {
    mMouseHandle = handle;
}

/*! Set projection type.
 *  \param proj : projection index.
 *  \sa enum_projection.
 */
void
OpenGL3dModel::setProjection(enum_projection    proj) {
    mProjection = proj;
    mParams->opengl_projection = (int)proj;
    updateGL();
}

/*! Set field of view.
 *  \param fov : field of view.
 */
void
OpenGL3dModel::setFieldOfView(double fov) {
    mCamera.setFovY(fov);
    mParams->opengl_fov = fov;
    updateGL();
}

/*! Get field of view.
 *  \return fov.
 */
double
OpenGL3dModel::getFieldOfView() {
    return mCamera.getFovY();
}

/*! Set camera position.
 *  \param pos : camera position.
 */
void
OpenGL3dModel::setCameraPos(m4d::vec3 pos) {
    mCamera.setEyePos(pos);
    mParams->opengl_eye_pos = pos;
    updateGL();
}

/*! Get camera position.
 *  \return vec3 : camera position.
 */
m4d::vec3
OpenGL3dModel::getCameraPos() {
    return mCamera.getEyePos();
}

/*! Set camera's point of interest.
 *  \param poi : camera's point of interest.
 */
void
OpenGL3dModel::setCameraPoi(m4d::vec3 poi) {
    mCamera.setPOI(poi);
    mParams->opengl_eye_poi = poi;
    updateGL();
}

/*! Get camera's point of interest.
 *  \return vec3 : camera's point of interest.
 */
m4d::vec3
OpenGL3dModel::getCameraPoi() {
    return mCamera.getPOI();
}

void OpenGL3dModel::setCameraVup(m4d::vec3 vup) {
    m4d::vec3 viewDir = mCamera.getDir();
    m4d::vec3 viewRight = viewDir^vup;

    mParams->opengl_eye_vup = (viewRight^viewDir).getNormalized();
    mCamera.setVup(mParams->opengl_eye_vup);
    updateGL();
}

m4d::vec3 OpenGL3dModel::getCameraVup() {
    return mParams->opengl_eye_vup;
}

/*!
 */
void
OpenGL3dModel::setCameraDirs(m4d::vec3 pos, m4d::vec3 poi, m4d::vec3 vup) {
    mCamera.setEyePos(pos);
    mCamera.setPOI(poi);

    m4d::vec3 dir = poi - pos;
    dir.normalize();

    mCamera.setDir(dir);
    mCamera.setVup(vup);
}

/*!
 */
void
OpenGL3dModel::getCameraDirs(m4d::vec3 &pos, m4d::vec3 &poi, m4d::vec3 &vup) {
    pos = mCamera.getEyePos();
    poi = mCamera.getPOI();
    vup = mCamera.getVup();
}

void
OpenGL3dModel::setCameraSphere(double theta, double phi, double dist) {
    mCamera.moveOnSphere(theta, phi, dist);
}

void
OpenGL3dModel::getCameraSphere(double &theta, double &phi, double &dist) {
    mCamera.getSphericalEyePos(theta, phi, dist);
}

/*!
 *  \param type : index of camera predefined values.
 *  \sa enum_camera_predefs.
 */
void
OpenGL3dModel::setCameraPredefs(enum_camera_predefs  type) {
    m4d::vec3 eye = mCamera.getEyePos();
    m4d::vec3 dir;
    m4d::vec3 vup;

    double dist = eye.getNorm();
    switch (type) {
        case enum_camera_xy:
            eye = m4d::vec3(0.0, 0.0, dist);
            dir = m4d::vec3(0.0, 0.0, -1.0);
            vup = m4d::vec3(0.0, 1.0, 0.0);
            break;
        case enum_camera_zx:
            eye = m4d::vec3(0.0, dist, 0.0);
            dir = m4d::vec3(0.0, -1.0, 0.0);
            vup = m4d::vec3(1.0, 0.0, 0.0);
            break;
        case enum_camera_yz:
            eye = m4d::vec3(dist, 0.0, 0.0);
            dir = m4d::vec3(-1.0, 0.0, 0.0);
            vup = m4d::vec3(0.0, 0.0, 1.0);
            break;
        case enum_camera_custom:
            double sr3 = 1.0 / sqrt(3.0);
            eye = dist * sr3 * m4d::vec3(1.0, 1.0, 1.0);
            dir = sr3 * m4d::vec3(-1.0, -1.0, -1.0);
            vup = sr3 * m4d::vec3(-1.0, -1.0, 1.0);
            break;
    }
    mCamera.setEyePos(eye);
    mCamera.setDir(dir);
    mCamera.setVup(vup);
    mCamera.setPOI(m4d::vec3());

    mParams->opengl_eye_pos = eye;
    mParams->opengl_eye_dir = dir;
    mParams->opengl_eye_vup = vup;
    mParams->opengl_eye_poi = m4d::vec3(0.0, 0.0, 0.0);

    updateGL();
}

/*! Clear all objects.
 */
void
OpenGL3dModel::clearAllObjects() {
    if (!mObjects.empty()) {
        mObjects.clear();
    }
    updateGL();
}

/*! Insert object.
 *  \param obj : pointer to object
 */
void
OpenGL3dModel::insertObject(MyObject* obj) {
    makeCurrent();
    MyObject* no = new MyObject(*obj);
    mObjects.push_back(no);
    updateGL();
}

/*! Set foreground color.
 *  \param  col : foreground color.
 */
void
OpenGL3dModel::setFGcolor(QColor col) {
    mFGcolor = col;
    updateGL();
}

/*! Set background color.
 *  \param  col : background color.
 */
void
OpenGL3dModel::setBGcolor(QColor col) {
    mBGcolor = col;
    updateGL();
}

/*! Set embedding color.
 *  \param  col : embedding color.
 */
void
OpenGL3dModel::setEmbColor(QColor col) {
    mEmbColor = col;
    updateGL();
}

/*! Set all colors.
 * \param fgcol : foreground color.
 * \param bgcol : background color.
 * \param embcol : embedding color.
 */
void
OpenGL3dModel::setColors(QColor fgcol, QColor bgcol, QColor embcol) {
    mBGcolor = bgcol;
    mFGcolor = fgcol;
    mEmbColor = embcol;
    updateGL();
}

/*! Set line width.
 *  \param width : line width/point size.
 */
void
OpenGL3dModel::setLineWidth(int width) {
    mLineWidth = width;
    updateGL();
}

/*! Set line anti-aliasing.
 *  \param smooth : 1=doAntialiasing
 */
void
OpenGL3dModel::setLineSmooth(int smooth) {
    mLineSmooth = smooth;
    updateGL();
}

/*! Set drawing style.
 *  \param  style : drawing style.
 *  \sa enum_draw_style.
 */
void
OpenGL3dModel::setStyle(enum_draw_style  style) {
    mDrawStyle = style;
}


/*! Set scaling factors.
 *  \param sx : scaling factor for x-direction.
 *  \param sy : scaling factor for y-direction.
 *  \param sz : scaling factor for z-direction.
 */
void
OpenGL3dModel::setScaling(double sx, double sy, double sz) {
    mScaleX = sx;
    mScaleY = sy;
    mScaleZ = sz;
    updateGL();
}

/*! Get scaling factors.
 *  \param sx : reference to scaling factor for x-direction.
 *  \param sy : reference to scaling factor for y-direction.
 *  \param sz : reference to scaling factor for z-direction.
 */
void
OpenGL3dModel::getScaling(double &sx, double &sy, double &sz) {
    sx = mScaleX;
    sy = mScaleY;
    sz = mScaleZ;
}

/*! Set camera to stereo/mono mode.
 *  \param useStereo : toggle stereo/mono mode.
 */
void
OpenGL3dModel::setStereoCam(bool useStereo) {
    mStereo = useStereo;
    mParams->opengl_stereo_use = (int)useStereo;
    updateGL();
}

/*! Set stereo parameters.
 *  \param sep : eye separation.
 *  \param type : type of stereo glasses.
 *  \sa enum_stereo_glasses.
 */
void
OpenGL3dModel::setStereoParams(double sep, enum_stereo_glasses type) {
    mCamera.setStereoParams(sep, type);
    mParams->opengl_stereo_glasses = type;
    mParams->opengl_stereo_sep     = sep;
    updateGL();
}

/*! Set stereo type.
 * \param type : stereo projection type.
 */
void
OpenGL3dModel::setStereoType(enum_stereo_type type) {
    mCamera.setStereoType(type);
    mParams->opengl_stereo_type = type;
    updateGL();
}

/*! Set artificial fog mode.
 *  \param useFog : use fog or not.
 */
void
OpenGL3dModel::setFog(bool useFog) {
    mUseFog = useFog;
    mParams->opengl_fog_use = (int)useFog;
    updateGL();
}

/*! Set artificial fog density.
 *  \param density : fog density.
 */
void
OpenGL3dModel::setFogDensity(double density) {
    mFogDensity = density;
    mParams->opengl_fog_init = mFogDensity;
    updateGL();
}

/*! Start or stop animation.
 *  \param  play : play or stop animation.
 */
void
OpenGL3dModel::playAnimRotation(bool play) {
    mPlayRot = play;
}

/*! Set animation parameters for rotation.
 *  \param rx : rotation value around x-axis;
 *  \param ry : rotation value around y-axis;
 *  \param rz : rotation value around z-axis;
 */
void
OpenGL3dModel::setAnimRotationParams(double rx, double ry, double rz) {
    mAnimRotX = rx;
    mAnimRotY = ry;
    mAnimRotZ = rz;
}

/*! Get animation parameters for rotation.
 *  \param rx : reference to rotation value around x-axis;
 *  \param ry : reference to rotation value around y-axis;
 *  \param rz : reference to rotation value around z-axis;
 */
void
OpenGL3dModel::getAnimRotationParams(double &rx, double &ry, double &rz) {
    rx = mAnimRotX;
    ry = mAnimRotY;
    rz = mAnimRotZ;
}

/*! Rotate camera.
 *  \param msec : milliseconds.
 *  \param local : rotate around local or global axes.
 */
void
OpenGL3dModel::doAnimRotation(double msec, bool local) {
    if (local) {
        mCamera.fixRotAroundVup(mAnimRotZ * msec * 1e-3);
        mCamera.fixRotAroundRight(mAnimRotX * msec * 1e-3);
        mCamera.fixRotAroundDir(mAnimRotY * msec * 1e-3);
    } else {
        mCamera.fixRotAroundZ(mAnimRotZ * msec * 1e-3);
        mCamera.fixRotAroundX(mAnimRotX * msec * 1e-3);
        mCamera.fixRotAroundY(mAnimRotY * msec * 1e-3);
    }
    updateGL();
}

/*! Set number of vertices do be drawn.
 *  \param num : number of vertices to be drawn.
 */
void
OpenGL3dModel::showNumVerts(int num) {
    mShowNumVerts = num;
    if (mShowNumVerts > mNumVerts) {
        mShowNumVerts = mNumVerts;
    }
    updateGL();
}

/*! Save rgb image.
 *  \param filename : image name.
 */
bool
OpenGL3dModel::saveRGBimage(QString filename) {
    unsigned char* buf = new unsigned char[DEF_OPENGL_WIDTH * DEF_OPENGL_HEIGHT * 4];

    updateGL();
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buf);

    QImage img(buf, DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT, QImage::Format_RGB888);
    QTransform trans;
    trans.scale(1.0, -1.0);
    QImage flippedImage = img.transformed(trans);
    if (!flippedImage.isNull()) {
        flippedImage.save(filename);
    }
    /*
    std::ofstream out(filename.toStdString().c_str());
    if (!out.is_open())
    {
    delete [] buf;
    return false;
    }

    out << "P6" << std::endl << DEF_OPENGL_WIDTH << " " << DEF_OPENGL_HEIGHT << std::endl << "255" << std::endl;
    int num;
    for(int row=DEF_OPENGL_HEIGHT-1; row>=0; row--)
    for(int col=0; col<DEF_OPENGL_WIDTH; col++)
    {
      num = (row*DEF_OPENGL_WIDTH+col)*4;
      out << char(buf[num+0]) << char(buf[num+1]) << char(buf[num+2]);
    }
    out << std::endl;
    out.close();
    */

    delete [] buf;
    return true;
}

/*!  Update viewing parameters.
 */
void
OpenGL3dModel::updateParams() {
    setCameraDirs(mParams->opengl_eye_pos, mParams->opengl_eye_poi, mParams->opengl_eye_vup);

    mProjection = (enum_projection)mParams->opengl_projection;
    mCamera.setFovY(mParams->opengl_fov);
    mLineWidth  = mParams->opengl_line_width;
    mLineSmooth = mParams->opengl_line_smooth;

    mBGcolor    = mParams->opengl_bg_color;
    mFGcolor    = mParams->opengl_line_color;
    mEmbColor   = mParams->opengl_emb_color;

    mStereo     = mParams->opengl_stereo_use;
    mCamera.setStereoParams(mParams->opengl_stereo_sep, mParams->opengl_stereo_glasses);
    mCamera.setStereoType(mParams->opengl_stereo_type);

    mUseFog     = mParams->opengl_fog_use;
    mFogDensity = mParams->opengl_fog_init;

    mScaleX = mParams->opengl_scale_x;
    mScaleY = mParams->opengl_scale_y;
    mScaleZ = mParams->opengl_scale_z;
    updateGL();
}

/*! Reset.
 */
void
OpenGL3dModel::reset() {
    clearAllObjects();
    clearEmbed();
    clearPoints();
    updateParams();
    updateGL();
}

// *********************************** protected methods ******************************
/*!
 *
 */
void
OpenGL3dModel::initializeGL() {
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    light_diffuse[0] = light_diffuse[1] = light_diffuse[2] = light_diffuse[3] = 1.0;
    light_position[0] = light_position[1] = light_position[2] = light_position[3] = 0.0;

    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

    mGLSLsupported = QGLShaderProgram::hasOpenGLShaderPrograms();

    fprintf(stderr, "Basic information about graphics board:\n");
    fprintf(stderr, "\tVendor:          %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "\tGPU:             %s\n", glGetString(GL_RENDERER));
    fprintf(stderr, "\tOpenGL-Version:  %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "\tGLSL:            %s\n\n", (mGLSLsupported ? "supported" : "not supported!"));


    shader = new QGLShaderProgram();
    //shader->addShaderFromSourceFile(QGLShader::Vertex,  "shader/vert.c");
    //shader->addShaderFromSourceFile(QGLShader::Fragment,"shader/frag.c");

    shader->addShaderFromSourceCode(QGLShader::Vertex, getVertexShaderCode());
    shader->addShaderFromSourceCode(QGLShader::Fragment, getFragmentShaderCode());
    shader->link();
    // std::cerr << "OpenGL frag log: " << shader->isLinked() << std::endl;
}

/*!
 */
void
OpenGL3dModel::paintGL() {
    switch (mProjection) {
        case enum_proj_perspective:
            mCamera.perspective();
            break;
        case enum_proj_orthographic:
            mCamera.orthographic();
            break;
    }
    mCamera.lookAtModelView();

    glClearColor(mBGcolor.redF(), mBGcolor.greenF(), mBGcolor.blueF(), 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mUseFog) {
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_EXP2);
        glFogf(GL_FOG_DENSITY, mFogDensity);
    } else {
        glDisable(GL_FOG);
    }

    mCamera.getEyePos(light_position);
    mCamera.getDir(light_direction);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);

    if (mStereo) {
        paintGL_stereo();
    } else {
        paintGL_mono();
    }

    glDisable(GL_FOG);
    paintGL_axes();
}


/*!
 *
 */
void
OpenGL3dModel::paintGL_mono() {
    glColor3f(1, 1, 1);
    glScalef(GLfloat(mScaleX), GLfloat(mScaleY), GLfloat(mScaleZ));

    //  GLfloat light_ambient[4] = { 0.4,0.4,0.4, 1.0 };
    GLfloat light_white[4] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_spec[4]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_black[4] = { 0.0, 0.0, 0.0, 1.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, light_black);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, light_black);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light_spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.5);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_white);


    /* -----------------------
    *   draw embedding
    * ----------------------- */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (mEmbVerts != NULL && mEmbIndices != NULL) {
        glEnable(GL_LINE_SMOOTH);
        if (mStereo) {
            glColor3f(1, 1, 1);
        } else {
            glColor3f(mEmbColor.redF(), mEmbColor.greenF(), mEmbColor.blueF());
        }
        glPointSize(1.0);
        glVertexPointer(3, GL_FLOAT, 0, mEmbVerts);
        glEnableClientState(GL_VERTEX_ARRAY);
#ifdef _WIN32
        for (unsigned int i = 0; i < mEmbCounter - 1; i++) {
            glDrawElements(GL_QUAD_STRIP, mCount[i], GL_UNSIGNED_INT, (const void**)mEmbIndices[i]);
        }
#else
#ifdef GL_GLEXT_PROTOTYPES
        glMultiDrawElements(GL_QUAD_STRIP, mCount, GL_UNSIGNED_INT, (const void**)mEmbIndices, mEmbCounter - 1);
#else
        for (unsigned int i = 0; i < mEmbCounter - 1; i++) {
            glDrawElements(GL_QUAD_STRIP, mCount[i], GL_UNSIGNED_INT, (const void**)mEmbIndices[i]);
        }
#endif // GL_GLEXT_PROTOTYPES
#endif
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_LINE_SMOOTH);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    /* -----------------------
    *   draw geodesic
    * ----------------------- */
    glLineWidth(mLineWidth);
    if (mLineSmooth == 1) {
        glEnable(GL_LINE_SMOOTH);
    }

    if (mStereo) {
        glColor3f(1, 1, 1);
    } else {
        glColor3f(mFGcolor.redF(), mFGcolor.greenF(), mFGcolor.blueF());
    }

    glPointSize(mLineWidth);
    glVertexPointer(3, GL_FLOAT, 0, mVerts);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (mDrawStyle == enum_draw_lines) {
        glDrawArrays(GL_LINE_STRIP, 0, mShowNumVerts);
    } else {
        glDrawArrays(GL_POINTS, 0, mShowNumVerts);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glLineWidth(1);
    glDisable(GL_LINE_SMOOTH);

    glDisable(GL_LIGHTING);

    glColor3f(1.0, 1.0, 1.0);

    /* -----------------------
    *   draw Sachs
    * ----------------------- */
    shader->bind();
    if (mStereo) {
        shader->setUniformValue(shader->uniformLocation("colortype"), 0);
    } else {
        shader->setUniformValue(shader->uniformLocation("colortype"), 2);
    }
    shader->setUniformValue(shader->uniformLocation("col1"), mParams->opengl_leg1_col1.redF(), mParams->opengl_leg1_col1.greenF(), mParams->opengl_leg1_col1.blueF());
    shader->setUniformValue(shader->uniformLocation("col2"), mParams->opengl_leg1_col2.redF(), mParams->opengl_leg1_col2.greenF(), mParams->opengl_leg1_col2.blueF());
    shader->setUniformValue(shader->uniformLocation("freq"), mParams->opengl_leg1_freq);
    shader->setUniformValue(shader->uniformLocation("useFog"), (int)mUseFog);
    shader->setUniformValue(shader->uniformLocation("fogFactor"), (GLfloat)(-mFogDensity * mFogDensity * 1.442695));
    shader->setUniformValue(shader->uniformLocation("eyePos"), light_position[0], light_position[1], light_position[2]);
    int num1, num2;

    if (mNumSachsVerts1 > 0) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < mShowNumVerts; i++) {
            num1 = 6 * i;
            num2 = 6 * i + 3;
            glTexCoord2f(0.0, mLambda[i]);
            glVertex3f(mSachsVerts1[num1 + 0], mSachsVerts1[num1 + 1], mSachsVerts1[num1 + 2]);
            glTexCoord2f(1.0, mLambda[i]);
            glVertex3f(mSachsVerts1[num2 + 0], mSachsVerts1[num2 + 1], mSachsVerts1[num2 + 2]);
        }
        glEnd();

        if (mStereo) {
            shader->setUniformValue(shader->uniformLocation("colortype"), 1);
        } else {
            shader->setUniformValue(shader->uniformLocation("colortype"), 2);
        }
        shader->setUniformValue(shader->uniformLocation("col1"), mParams->opengl_leg2_col1.redF(), mParams->opengl_leg2_col1.greenF(), mParams->opengl_leg2_col1.blueF());
        shader->setUniformValue(shader->uniformLocation("col2"), mParams->opengl_leg2_col2.redF(), mParams->opengl_leg2_col2.greenF(), mParams->opengl_leg2_col2.blueF());
        shader->setUniformValue(shader->uniformLocation("freq"), mParams->opengl_leg2_freq);
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < mShowNumVerts; i++) {
            num1 = 6 * i;
            num2 = 6 * i + 3;
            glTexCoord2f(0.0, mLambda[i]);
            glVertex3f(mSachsVerts2[num1 + 0], mSachsVerts2[num1 + 1], mSachsVerts2[num1 + 2]);
            glTexCoord2f(1.0, mLambda[i]);
            glVertex3f(mSachsVerts2[num2 + 0], mSachsVerts2[num2 + 1], mSachsVerts2[num2 + 2]);
        }
        glEnd();
    }
    shader->release();

    /* -----------------------
    *   draw objects
    * ----------------------- */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (mWiredObjs) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    for (unsigned int i = 0; i < mObjects.size(); i++) {
        if (mObjects[i]->withLight(light_diffuse, mStereo)) {
            //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light_diffuse);
            glEnable(GL_LIGHTING);
        }

        if (!mObjects[i]->drawObject(mStereo)) {
            if (mObjects[i]->getObjectType() == enum_object_text3d && mObjects[i]->getObjectDim() == enum_object_dim_3d) {
                float cx, cy, cz, size;
                mObjects[i]->getValue(0, cx);
                mObjects[i]->getValue(1, cy);
                mObjects[i]->getValue(2, cz);
                mObjects[i]->getValue(3, size);
                QFont font;
                font.setPixelSize((int)size);
                this->renderText((double)cx, (double)cy, (double)cz, QString(mObjects[i]->getText().c_str()), font);
            }
        }
        glDisable(GL_LIGHTING);
    }
}

/*!
 *
 */
void
OpenGL3dModel::paintGL_stereo() {
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);

    mCamera.lookAtMV_Left();
    paintGL_mono();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glDrawBuffer(GL_BACK);
    glClear(GL_DEPTH_BUFFER_BIT);
    mCamera.lookAtMV_Right();
    paintGL_mono();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

/*!
 *
 */
void
OpenGL3dModel::paintGL_axes() {
    mCamera.perspectiveAxes();
    glViewport(0, 0, (int)(double(DEF_OPENGL_HEIGHT) / 10.0), (int)(double(DEF_OPENGL_HEIGHT) / 10.0));
    glClear(GL_DEPTH_BUFFER_BIT);
    mCamera.lookAtCenterModelView();

    QColor textColor = Qt::white;
    if (mBGcolor.value() > 127) {
        textColor = Qt::black;
    }

    glEnable(GL_LIGHTING);

    GLfloat light_ambient[4] = {0.0, 0.0, 0.0, 1.0};
    GLfloat light_spec[4] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, light_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light_spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.5);


    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0);
    qglColor(textColor);
    renderText(0.0, 0.0, 1.2, QString("x"));
    light_spec[0] = 1.0;
    light_spec[1] = 0.0;
    light_spec[2] = 0.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light_spec);
    glColor3f(1.0f, 0.0f, 0.0f);
    gluCylinder(mQuadricAxis, 0.0625, 0.0625, 0.75, 32, 32);
    gluDisk(mQuadricDisk, 0.0, 0.0625, 32, 32);
    glTranslatef(0.0, 0.0, 0.75);
    gluCylinder(mQuadricAxis, 0.125, 0.0, 0.25, 32, 32);
    gluDisk(mQuadricDisk, 0.0, 0.125, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    qglColor(textColor);
    renderText(-0.1, 0.1, 1.2, QString("y"));
    light_spec[0] = 0.0;
    light_spec[1] = 1.0;
    light_spec[2] = 0.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light_spec);
    gluCylinder(mQuadricAxis, 0.0625, 0.0625, 0.75, 32, 32);
    gluDisk(mQuadricDisk, 0.0, 0.0625, 32, 32);
    glTranslatef(0.0, 0.0, 0.75);
    gluCylinder(mQuadricAxis, 0.125, 0.0, 0.25, 32, 32);
    gluDisk(mQuadricDisk, 0.0, 0.125, 32, 32);
    glPopMatrix();


    glPushMatrix();
    qglColor(textColor);
    renderText(-0.1, -0.1, 1.2, mNameOfZaxis);
    light_spec[0] = 0.0;
    light_spec[1] = 0.0;
    light_spec[2] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light_spec);
    gluCylinder(mQuadricAxis, 0.0625, 0.0625, 0.75, 32, 32);
    gluDisk(mQuadricDisk, 0.0, 0.0625, 32, 32);
    glTranslatef(0.0, 0.0, 0.75);
    gluCylinder(mQuadricAxis, 0.125, 0.0, 0.25, 32, 32);
    gluDisk(mQuadricDisk, 0.0, 0.125, 32, 32);
    glPopMatrix();

    glDisable(GL_LIGHTING);
}



/*!
 *  \param event : key event.
 */
void
OpenGL3dModel::keyPressEvent(QKeyEvent* event) {
    mKeyPressed = event->key();

    if (mKeyPressed == Qt::Key_I) {
        mCamera.setEyePos(m4d::vec3(DEF_INIT_EYE_POS_X, DEF_INIT_EYE_POS_Y, DEF_INIT_EYE_POS_Z));
        mCamera.setPOI(m4d::vec3(DEF_INIT_POI_X, DEF_INIT_POI_Y, DEF_INIT_POI_Z));
        mCamera.setVup(m4d::vec3(DEF_INIT_VUP_X, DEF_INIT_VUP_Y, DEF_INIT_VUP_Z));
        mCamera.setFovY(DEF_INIT_FOV);
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_X) {
        m4d::vec3 pos = mCamera.getEyePos();
        mCamera.setEyePos(m4d::vec3(pos.getNorm(), 0.0, 0.0));
        mCamera.setPOI(m4d::vec3(0.0, 0.0, 0.0));
        mCamera.setVup(m4d::vec3(0.0, 0.0, 1.0));
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_Y) {
        m4d::vec3 pos = mCamera.getEyePos();
        mCamera.setEyePos(m4d::vec3(0.0, pos.getNorm(), 0.0));
        mCamera.setPOI(m4d::vec3(0.0, 0.0, 0.0));
        mCamera.setVup(m4d::vec3(1.0, 0.0, 0.0));
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_Z) {
        m4d::vec3 pos = mCamera.getEyePos();
        mCamera.setEyePos(m4d::vec3(0.0, 0.0, pos.getNorm()));
        mCamera.setPOI(m4d::vec3(0.0, 0.0, 0.0));
        mCamera.setVup(m4d::vec3(0.0, 1.0, 0.0));
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_Up) {
        m4d::vec3 pos = mCamera.getEyePos();
        pos = pos + 0.5 * mCamera.getDir();
        mCamera.setEyePos(pos);
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_Down) {
        m4d::vec3 pos = mCamera.getEyePos();
        pos = pos - 0.5 * mCamera.getDir();
        mCamera.setEyePos(pos);
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_PageUp) {
        m4d::vec3 pos = mCamera.getEyePos();
        pos = pos + 5.0 * mCamera.getDir();
        mCamera.setEyePos(pos);
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_PageDown) {
        m4d::vec3 pos = mCamera.getEyePos();
        pos = pos - 5.0 * mCamera.getDir();
        mCamera.setEyePos(pos);
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_Less) {
        mParams->opengl_fov = mCamera.getFovY() - 0.5;
        mCamera.setFovY(mParams->opengl_fov);
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_Greater) {
        mParams->opengl_fov = mCamera.getFovY() + 0.5;
        mCamera.setFovY(mParams->opengl_fov);
        emit cameraMoved();
    } else if (mKeyPressed == Qt::Key_W) {
        mWiredObjs = !mWiredObjs;
    }
#if 0
    else if (mKeyPressed == Qt::Key_R) {
        if (shader->isLinked()) {
            shader->removeAllShaders();
        }
        shader->addShaderFromSourceFile(QGLShader::Vertex, "shader/vert.c");
        shader->addShaderFromSourceFile(QGLShader::Fragment, "shader/frag.c");
        shader->link();
    }
#endif
    updateGL();
}

/*!
 *  \param event : key event.
 */
void
OpenGL3dModel::keyReleaseEvent(QKeyEvent* event) {
    mKeyPressed = Qt::Key_No;
    event->ignore();
}

/*!
 *  \param event : mouse event.
 */
void
OpenGL3dModel::mousePressEvent(QMouseEvent * event) {
    mButtonPressed = event->button();
    mLastPos = event->pos();
}

/*!
 *  \param event : mouse event.
 */
void
OpenGL3dModel::mouseReleaseEvent(QMouseEvent * event) {
    mButtonPressed = Qt::NoButton;
    event->accept();
}

/*!
 *  \param event : mouse event.
 */
void
OpenGL3dModel::mouseMoveEvent(QMouseEvent * event) {
    QPoint dxy = event->pos() - mLastPos;
    mLastPos   = event->pos();

    switch (mMouseHandle) {
        /* ------------------------------
          *      rotate local
          * ------------------------------ */
        case enum_mouse_rotate: {
            if (mButtonPressed == DEF_CAM_ROT_LOCAL_VUP) {
                double angle = DEF_CAM_ROT_LOCAL_VUP_SCALE * M_PI * DEF_CAM_ROT_LOCAL_VUP_XY;
                mCamera.fixRotAroundVup(angle);
            } else if (mButtonPressed == DEF_CAM_ROT_LOCAL_RIGHT) {
                double angle = DEF_CAM_ROT_LOCAL_RIGHT_SCALE * M_PI * DEF_CAM_ROT_LOCAL_RIGHT_XY;
                mCamera.fixRotAroundRight(angle);
            } else if (mButtonPressed == DEF_CAM_ROT_LOCAL_DIR) {
                double angle = DEF_CAM_ROT_LOCAL_DIR_SCALE * M_PI * DEF_CAM_ROT_LOCAL_DIR_XY;
                mCamera.fixRotAroundDir(angle);
            }
            emit cameraMoved();
            break;
        }
        /* ------------------------------
        *      rotate global
        * ------------------------------ */
        case enum_mouse_rotate_global: {
            if (mButtonPressed == DEF_CAM_ROT_GLOBAL_Z) {
                double angle = DEF_CAM_ROT_GLOBAL_Z_SCALE * M_PI * DEF_CAM_ROT_GLOBAL_Z_XY;
                mCamera.fixRotAroundZ(angle);
            } else if (mButtonPressed == DEF_CAM_ROT_GLOBAL_Y) {
                double angle = DEF_CAM_ROT_GLOBAL_Y_SCALE * M_PI * DEF_CAM_ROT_GLOBAL_Y_XY;
                mCamera.fixRotAroundY(angle);
            } else if (mButtonPressed == DEF_CAM_ROT_GLOBAL_X) {
                double angle = DEF_CAM_ROT_GLOBAL_X_SCALE * M_PI * DEF_CAM_ROT_GLOBAL_X_XY;
                mCamera.fixRotAroundX(angle);
            }
            emit cameraMoved();
            break;
        }
        /* ------------------------------
        *      rotate on sphere
        * ------------------------------ */
        case enum_mouse_rotate_sphere: {
            double theta, phi, dist;
            mCamera.getSphericalEyePos(theta, phi, dist);
            if (mButtonPressed == DEF_CAM_ROT_SPHERE_THETA_PHI) {
                theta -= dxy.y() * DEF_CAM_ROT_SPHERE_TP_SCALE;
                phi   -= dxy.x() * DEF_CAM_ROT_SPHERE_TP_SCALE;
                mCamera.moveOnSphere(theta, phi, dist);
            } else if (mButtonPressed == DEF_CAM_ROT_SPHERE_DISTANCE) {
                dist += dxy.y() * DEF_CAM_ROT_SPHERE_DIST_SCALE;
                mCamera.moveOnSphere(theta, phi, dist);
            } else if (mButtonPressed == Qt::RightButton) {
                dist += dxy.y() * DEF_CAM_ROT_SPHERE_DIST_SCALE * 0.4 * dist;
                mCamera.moveOnSphere(theta, phi, dist);
            }
            emit cameraMoved();
            break;
        }
        /* ------------------------------
        *      move local
        * ------------------------------ */
        case enum_mouse_camera_dist: {
            m4d::vec3 pos = mCamera.getEyePos();
            pos = pos + 0.05 * dxy.y() * mCamera.getDir();
            mCamera.setEyePos(pos);
            emit cameraMoved();
            break;
        }
        /* ------------------------------
        *      move local
        * ------------------------------ */
        case enum_mouse_move_local: {
            if (mButtonPressed == Qt::LeftButton) {
                m4d::vec3 pos = mCamera.getEyePos();
                m4d::vec3 poi = mCamera.getPOI();
                pos = pos + 0.01 * (dxy.y() * mCamera.getVup() - dxy.x() * mCamera.getRight());
                poi = poi + 0.01 * (dxy.y() * mCamera.getVup() - dxy.x() * mCamera.getRight());
                mCamera.setEyePos(pos);
                mCamera.setPOI(poi);
            } else if (mButtonPressed == Qt::MidButton) {
                m4d::vec3 pos = mCamera.getEyePos();
                m4d::vec3 poi = mCamera.getPOI();
                pos = pos + 0.05 * dxy.y() * mCamera.getDir();
                poi = poi + 0.05 * dxy.y() * mCamera.getDir();
                mCamera.setEyePos(pos);
                mCamera.setPOI(poi);
            }
            emit cameraMoved();
            break;
        }
        /* ------------------------------
        *      move global
        * ------------------------------ */
        case enum_mouse_move_global: {
            if (mButtonPressed == Qt::LeftButton) {
                m4d::vec3 pos = mCamera.getEyePos();
                m4d::vec3 poi = mCamera.getPOI();
                pos = pos + 0.01 * m4d::vec3(-dxy.x(), dxy.y(), 0.0);
                poi = poi + 0.01 * m4d::vec3(-dxy.x(), dxy.y(), 0.0);
                mCamera.setEyePos(pos);
                mCamera.setPOI(poi);
            } else if (mButtonPressed == Qt::MidButton) {
                m4d::vec3 pos = mCamera.getEyePos();
                m4d::vec3 poi = mCamera.getPOI();
                pos = pos + 0.01 * m4d::vec3(0.0, 0.0, dxy.y());
                poi = poi + 0.01 * m4d::vec3(0.0, 0.0, dxy.y());
                mCamera.setEyePos(pos);
                mCamera.setPOI(poi);
            }
            emit cameraMoved();
            break;
        }
        /* ------------------------------
        *      move poi
        * ------------------------------ */
        case enum_mouse_move_poi: {
            if (mButtonPressed == Qt::LeftButton) {
                m4d::vec3 poi = mCamera.getPOI();
                poi = poi + 0.01 * m4d::vec3(-dxy.x(), dxy.y(), 0.0);
                mCamera.setPOI(poi);
            } else if (mButtonPressed == Qt::MidButton) {
                m4d::vec3 poi = mCamera.getPOI();
                poi = poi + 0.01 * m4d::vec3(0.0, 0.0, dxy.y());
                mCamera.setPOI(poi);
            }
            emit cameraMoved();
            break;
        }
    }
    updateGL();
}

/*! Get vertex shader code.
 */
QString
OpenGL3dModel::getVertexShaderCode() {
    QString vert;
    vert += "uniform int  useFog;\n";
    vert += "uniform vec3 eyePos;\n";

    vert += "void main()\n";
    vert += "{\n";
    vert += "   gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n";
    vert += "   gl_TexCoord[0] = gl_MultiTexCoord0;\n";

    vert += "   if (useFog==1)\n";
    vert += "     gl_FogFragCoord = length(eyePos-gl_Vertex.xyz);\n";
    vert += "}\n";

    return vert;
}


/*! Get fragment shader code.
 */
QString
OpenGL3dModel::getFragmentShaderCode() {
    QString frag;
    frag += "uniform int   useFog;\n";
    frag += "uniform float fogFactor;\n";
    frag += "uniform vec3  col1;\n";
    frag += "uniform vec3  col2;\n";
    frag += "uniform float freq;\n";
    frag += "uniform int   colortype;\n";
    frag += "void main()\n";
    frag += "{\n";
    frag += "vec2 tc = gl_TexCoord[0].st;\n";

    frag += "float val1 = 0.5*(sin(tc.y*freq)+1.0);\n";
    frag += "float val2 = 0.5*(cos(tc.y*freq+1.5707963)+1.0);\n";

    frag += "vec4 color = vec4(0.0,0.0,0.0,1.0);\n";
    frag += "if (colortype==0)\n";
    frag += "  color.rgb = vec3(val1,val1,val1);\n";
    frag += "else if (colortype==1)\n";
    frag += "  color.rgb = vec3(val2,val2,val2);\n";
    frag += "else if (colortype==2)\n";
    frag += "  color.rgb = col1*val1 + col2*val2;\n";

    frag += "if (useFog==1)\n";
    frag += "{\n";
    frag += "  float fog = exp2( gl_FogFragCoord*gl_FogFragCoord*fogFactor );\n";
    frag += "  fog = clamp(fog,0.0,1.0);\n";
    frag += "  color = mix(vec4(0.0,0.0,0.0,1.0),color,fog);\n";
    frag += "}\n";
    frag += "gl_FragColor = color;\n";
    frag += "}\n";

    return frag;
}
