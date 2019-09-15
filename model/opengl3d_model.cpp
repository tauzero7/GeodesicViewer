/**
 * @file    opengl3d_model.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include <fstream>
#include <QApplication>
#include <QDesktopWidget>
#include "opengl3d_model.h"
#include <math/TransCoordinates.h>

extern m4d::Object mObject;


OpenGL3dModel::OpenGL3dModel(struct_params* par, QWidget* parent)
    : QOpenGLWidget(parent) {
    mParams = par;

    mCamera.setSize(DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT);
    mCamera.setEyePos(m4d::vec3(&mParams->opengl_eye_pos[0]));
    mCamera.setPOI(m4d::vec3(&mParams->opengl_eye_poi[0]));
    mCamera.setVup(m4d::vec3(&mParams->opengl_eye_vup[0]));
    mCamera.setFovY(mParams->opengl_fov);

    mButtonPressed = Qt::NoButton;
    setFocusPolicy(Qt::ClickFocus);
    mKeyPressed = Qt::Key_No;

    mBGcolor = mParams->opengl_bg_color;
    mFGcolor = mParams->opengl_line_color;

    mEmbColor  = Qt::lightGray;
    mLineWidth = mParams->opengl_line_width;

    mMouseHandle = enum_mouse_rotate;
    mProjection  = static_cast<enum_projection>(mParams->opengl_projection);
    mDrawStyle   = enum_draw_lines;

    // Vertices for the geodesic.
    mVerts        = nullptr;
    mLambda       = nullptr;
    mNumVerts     = 0;
    mShowNumVerts = 0;

    mSachsLegs      = mParams->opengl_sachs_legs;
    mSachsScale     = mParams->opengl_sachs_scale;
    mSachsVerts1    = nullptr;
    mSachsVerts2    = nullptr;
    mNumSachsVerts1 = 0;
    mNumSachsVerts2 = 0;

    // Data structures for an embedding diagram.
    mEmbVerts    = nullptr;
    mEmbNumVerts = 0;
    mEmbIndices  = nullptr;
    mEmbCounter  = 0;
    mCount       = nullptr;


    mScaleX = mScaleY = mScaleZ = 1.0;

    mStereo = (mParams->opengl_stereo_use == 1);
    mUseFog = (mParams->opengl_fog_use == 1);
    mFogDensity = mParams->opengl_fog_init; //DEF_OPENGL_FOG_DENSITY_INIT;
    //QGLFormat f = format();

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
    if (mQuadricAxis != nullptr) {
        gluDeleteQuadric(mQuadricAxis);
    }
    mQuadricAxis = nullptr;
    if (mQuadricDisk != nullptr) {
        gluDeleteQuadric(mQuadricDisk);
    }
    mQuadricDisk = nullptr;
}

// ************************************ public methods ********************************
/*! Set geodesic points.
 *  \param  data : pointer to object.
 *  \param  dtype : type of drawing.
 *  \param  update : update gl.
 */
void
OpenGL3dModel::setPoints(m4d::enum_draw_type  dtype, bool needUpdate) {
    if (mVerts != nullptr) {
        delete [] mVerts;
    }
    if (mLambda != nullptr) {
        delete [] mLambda;
    }

    mNumVerts = static_cast<int>(mObject.points.size());
    mVerts    = new GLfloat[ mNumVerts * 3 ];
    mLambda   = new GLfloat[ mNumVerts ];

    GLfloat* vptr = mVerts;
    GLfloat* lptr = mLambda;

    mNameOfZaxis = QString("z");

    m4d::vec4 tp;
    for(int i = 0; i < mNumVerts; i++) {
        size_t idx = static_cast<size_t>(i);
        switch (dtype) {
            case m4d::enum_draw_pseudocart:
                mObject.currMetric->transToPseudoCart(mObject.points[idx], tp);
                break;
            case m4d::enum_draw_coordinates:
                break;
            case m4d::enum_draw_embedding:
                mObject.currMetric->transToEmbedding(mObject.points[idx], tp);
                break;
            case m4d::enum_draw_twoplusone:
                mObject.currMetric->transToTwoPlusOne(mObject.points[idx], tp);
                mNameOfZaxis = QString("t");
                break;
            case m4d::enum_draw_effpoti:
                break;
            case m4d::enum_draw_custom:
                mObject.currMetric->transToCustom(mObject.points[idx], tp);
                break;
        }
        *(vptr++) = GLfloat(tp[1]);
        *(vptr++) = GLfloat(tp[2]);
        *(vptr++) = GLfloat(tp[3]);

        *(lptr++) = GLfloat(mObject.lambda[idx]);
    }

    mShowNumVerts = mNumVerts;
    if (needUpdate) {
        update();
    }
}


void OpenGL3dModel::setSachsAxes(bool needUpdate) {
    if (mSachsVerts1 != nullptr) {
        delete [] mSachsVerts1;
    }
    if (mSachsVerts2 != nullptr) {
        delete [] mSachsVerts2;
    }

    mNumSachsVerts1 = static_cast<int>(mObject.sachs1.size());
    mNumSachsVerts2 = static_cast<int>(mObject.sachs2.size());
    int mv = static_cast<int>(mObject.points.size());

    mSachsVerts1 = new GLfloat[ mNumSachsVerts1 * 3 * 2 ];
    mSachsVerts2 = new GLfloat[ mNumSachsVerts2 * 3 * 2 ];

    GLfloat* vptr1 = mSachsVerts1;
    GLfloat* vptr2 = mSachsVerts2;

    m4d::vec4 tp;
    m4d::vec4 d1, d2, s1, s2;

    double f, l1a, l1b, l2a, l2b;
    int n1 = 0, n2 = 0;
    while (n1 < mNumSachsVerts1 && n2 < mNumSachsVerts2 && n1 < mv) {
        size_t ni1 = static_cast<size_t>(n1);
        size_t ni2 = static_cast<size_t>(n2);
        // Umparametrisierung
        f = mObject.sachs1[ni1].x(0) / mObject.dirs[ni1].x(0);
        s1 = mObject.sachs1[ni1] - f * mObject.dirs[ni1];
        f = mObject.sachs2[ni2].x(0) / mObject.dirs[ni2].x(0);
        s2 = mObject.sachs2[ni2] - f * mObject.dirs[ni2];

        m4d::TransCoordinates::toCartesianCoord(mObject.currMetric->getCoordType(), mObject.points[ni1], s1, tp, d1);
        m4d::TransCoordinates::toCartesianCoord(mObject.currMetric->getCoordType(), mObject.points[ni1], s2, tp, d2);

        d1 *= mParams->opengl_sachs_scale;
        d2 *= mParams->opengl_sachs_scale;

        switch (mParams->opengl_sachs_legs) {
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

    if (needUpdate) {
        update();
    }
}


void OpenGL3dModel::clearSachsAxes() {
    if (mSachsVerts1 != nullptr) {
        delete [] mSachsVerts1;
    }
    if (mSachsVerts2 != nullptr) {
        delete [] mSachsVerts2;
    }

    mSachsVerts1 = mSachsVerts2 = nullptr;
    mNumSachsVerts1 = mNumSachsVerts2 = 0;
    update();
}


void OpenGL3dModel::clearPoints() {
    if (mVerts != nullptr) {
        delete [] mVerts;
    }
    mVerts = nullptr;

    if (mLambda != nullptr) {
        delete [] mLambda;
    }
    mLambda = nullptr;

    mNumVerts = 0;
    mShowNumVerts = 0;
    update();
}


void OpenGL3dModel::genEmbed(m4d::Metric* currMetric) {
    if (mEmbVerts != nullptr) {
        delete [] mEmbVerts;
    }
    mEmbVerts = nullptr;

    if (mEmbIndices != nullptr) {
        for (unsigned int j = 0; j < mEmbCounter; j++) {
            delete [] mEmbIndices[j];
        }
        delete [] mEmbIndices;
    }
    mEmbIndices = nullptr;
    mEmbNumElems = mEmbCounter = 0;

    if (mCount != nullptr) {
        delete [] mCount;
    }
    mCount = nullptr;

    std::vector<m4d::vec3> vertices;
    std::vector<int>       indices;

    if (currMetric == nullptr) {
        return;
    }

    if (currMetric->getEmbeddingVertices(vertices, indices, mEmbNumElems, mEmbCounter) == 0) {
        return;
    }

    mEmbNumVerts = static_cast<int>(vertices.size());

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


    mEmbVerts = new float[ mEmbNumVerts * 3 ];
    float* vptr = mEmbVerts;

    for(int i = 0; i < mEmbNumVerts; i++) {
        size_t idx = static_cast<size_t>(i);
        *(vptr++) = static_cast<float>(vertices[idx][0]);
        *(vptr++) = static_cast<float>(vertices[idx][1]);
        *(vptr++) = static_cast<float>(vertices[idx][2]);
    }
    update();
}


void OpenGL3dModel::clearEmbed() {
    if (mEmbVerts != nullptr) {
        delete [] mEmbVerts;
    }
    mEmbVerts = nullptr;
    mEmbNumVerts = 0;

    if (mEmbIndices != nullptr) {
        for (unsigned int j = 0; j < mEmbCounter; j++) {
            delete [] mEmbIndices[j];
        }
        delete [] mEmbIndices;
    }
    mEmbIndices = nullptr;

    if (mCount != nullptr) {
        delete [] mCount;
    }
    mCount = nullptr;

    mEmbNumElems = mEmbCounter = 0;
    update();
}


void OpenGL3dModel::setMouseHandle(enum_mouse_handle  handle) {
    mMouseHandle = handle;
}


void OpenGL3dModel::setProjection(enum_projection proj) {
    mProjection = proj;
    mParams->opengl_projection = static_cast<int>(proj);
    update();
}


void OpenGL3dModel::setFieldOfView(double fov) {
    mCamera.setFovY(fov);
    mParams->opengl_fov = fov;
    update();
}


double OpenGL3dModel::getFieldOfView() {
    return mCamera.getFovY();
}


void OpenGL3dModel::setCameraPos(m4d::vec3 pos) {
    mCamera.setEyePos(pos);
    mParams->opengl_eye_pos = pos;
    update();
}


m4d::vec3 OpenGL3dModel::getCameraPos() {
    return mCamera.getEyePos();
}


void OpenGL3dModel::setCameraPoi(m4d::vec3 poi) {
    mCamera.setPOI(poi);
    mParams->opengl_eye_poi = poi;
    update();
}


m4d::vec3
OpenGL3dModel::getCameraPoi() {
    return mCamera.getPOI();
}


void OpenGL3dModel::setCameraVup(m4d::vec3 vup) {
    m4d::vec3 viewDir = mCamera.getDir();
    m4d::vec3 viewRight = viewDir^vup;

    mParams->opengl_eye_vup = (viewRight^viewDir).getNormalized();
    mCamera.setVup(mParams->opengl_eye_vup);
    update();
}


m4d::vec3 OpenGL3dModel::getCameraVup() {
    return mParams->opengl_eye_vup;
}


void
OpenGL3dModel::setCameraDirs(m4d::vec3 pos, m4d::vec3 poi, m4d::vec3 vup) {
    mCamera.setEyePos(pos);
    mCamera.setPOI(poi);

    m4d::vec3 dir = poi - pos;
    dir.normalize();

    mCamera.setDir(dir);
    mCamera.setVup(vup);
}


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

    update();
}


void OpenGL3dModel::clearAllObjects() {
    if (!mObjects.empty()) {
        mObjects.clear();
    }
    update();
}


void OpenGL3dModel::insertObject(MyObject* obj) {
    makeCurrent();
    MyObject* no = new MyObject(*obj);
    mObjects.push_back(no);
    update();
}


void OpenGL3dModel::setFGcolor(QColor col) {
    mFGcolor = col;
    update();
}


void OpenGL3dModel::setBGcolor(QColor col) {
    mBGcolor = col;
    update();
}


void OpenGL3dModel::setEmbColor(QColor col) {
    mEmbColor = col;
    update();
}


void OpenGL3dModel::setColors(QColor fgcol, QColor bgcol, QColor embcol) {
    mBGcolor = bgcol;
    mFGcolor = fgcol;
    mEmbColor = embcol;
    update();
}


void OpenGL3dModel::setLineWidth(int width) {
    mLineWidth = width;
    update();
}


void OpenGL3dModel::setLineSmooth(int smooth) {
    mLineSmooth = smooth;
    update();
}


void OpenGL3dModel::setStyle(enum_draw_style  style) {
    mDrawStyle = style;
}


void OpenGL3dModel::setScaling(double sx, double sy, double sz) {
    mScaleX = sx;
    mScaleY = sy;
    mScaleZ = sz;
    update();
}

void OpenGL3dModel::getScaling(double &sx, double &sy, double &sz) {
    sx = mScaleX;
    sy = mScaleY;
    sz = mScaleZ;
}


void OpenGL3dModel::setStereoCam(bool useStereo) {
    mStereo = useStereo;
    mParams->opengl_stereo_use = static_cast<int>(useStereo);
    // updateGL();
}

void OpenGL3dModel::setStereoParams(double sep, enum_stereo_glasses type) {
    mCamera.setStereoParams(sep, type);
    mParams->opengl_stereo_glasses = type;
    mParams->opengl_stereo_sep     = sep;
    update();
}


void OpenGL3dModel::setStereoType(enum_stereo_type type) {
    mCamera.setStereoType(type);
    mParams->opengl_stereo_type = type;
    update();
}


void OpenGL3dModel::setFog(bool useFog) {
    mUseFog = useFog;
    mParams->opengl_fog_use = static_cast<int>(useFog);
    update();
}


void OpenGL3dModel::setFogDensity(double density) {
    mFogDensity = density;
    mParams->opengl_fog_init = mFogDensity;
    update();
}


void OpenGL3dModel::playAnimRotation(bool play) {
    mPlayRot = play;
}


void OpenGL3dModel::setAnimRotationParams(double rx, double ry, double rz) {
    mAnimRotX = rx;
    mAnimRotY = ry;
    mAnimRotZ = rz;
}


void OpenGL3dModel::getAnimRotationParams(double &rx, double &ry, double &rz) {
    rx = mAnimRotX;
    ry = mAnimRotY;
    rz = mAnimRotZ;
}


void OpenGL3dModel::doAnimRotation(double msec, bool local) {
    if (local) {
        mCamera.fixRotAroundVup(mAnimRotZ * msec * 1e-3);
        mCamera.fixRotAroundRight(mAnimRotX * msec * 1e-3);
        mCamera.fixRotAroundDir(mAnimRotY * msec * 1e-3);
    } else {
        mCamera.fixRotAroundZ(mAnimRotZ * msec * 1e-3);
        mCamera.fixRotAroundX(mAnimRotX * msec * 1e-3);
        mCamera.fixRotAroundY(mAnimRotY * msec * 1e-3);
    }
    update();
}


void OpenGL3dModel::showNumVerts(int num) {
    mShowNumVerts = num;
    if (mShowNumVerts > mNumVerts) {
        mShowNumVerts = mNumVerts;
    }
    update();
}


bool OpenGL3dModel::saveRGBimage(QString filename) {
    unsigned char* buf = new unsigned char[DEF_OPENGL_WIDTH * DEF_OPENGL_HEIGHT * 4];

    update();
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


void OpenGL3dModel::updateParams() {
    setCameraDirs(mParams->opengl_eye_pos, mParams->opengl_eye_poi, mParams->opengl_eye_vup);

    mProjection = static_cast<enum_projection>(mParams->opengl_projection);
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
    update();
}


void OpenGL3dModel::reset() {
    clearAllObjects();
    clearEmbed();
    clearPoints();
    updateParams();
    update();
}


void OpenGL3dModel::initializeGL() {
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

    mGLSLsupported = QOpenGLShaderProgram::hasOpenGLShaderPrograms();

    fprintf(stderr, "Basic information about graphics board:\n");
    fprintf(stderr, "\tVendor:          %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "\tGPU:             %s\n", glGetString(GL_RENDERER));
    fprintf(stderr, "\tOpenGL-Version:  %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "\tGLSL:            %s\n\n", (mGLSLsupported ? "supported" : "not supported!"));


    shader = new QOpenGLShaderProgram();
    //shader->addShaderFromSourceFile(QGLShader::Vertex,  "shader/vert.c");
    //shader->addShaderFromSourceFile(QGLShader::Fragment,"shader/frag.c");

    shader->addShaderFromSourceCode(QOpenGLShader::Vertex, getVertexShaderCode());
    shader->addShaderFromSourceCode(QOpenGLShader::Fragment, getFragmentShaderCode());
    shader->link();
    // std::cerr << "OpenGL frag log: " << shader->isLinked() << std::endl;

    mDPIFactor[0] = QApplication::desktop()->devicePixelRatioF();
    mDPIFactor[1] = QApplication::desktop()->devicePixelRatioF();
}


void OpenGL3dModel::paintGL() {
    switch (mProjection) {
        case enum_proj_perspective:
            mCamera.perspective();
            break;
        case enum_proj_orthographic:
            mCamera.orthographic();
            break;
    }
    mCamera.lookAtModelView();

    glClearColor(static_cast<float>(mBGcolor.redF()), static_cast<float>(mBGcolor.greenF()),
                 static_cast<float>(mBGcolor.blueF()), 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mUseFog) {
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_EXP2);
        glFogf(GL_FOG_DENSITY, static_cast<float>(mFogDensity));
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


void OpenGL3dModel::paintGL_mono() {
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

    if (mEmbVerts != nullptr && mEmbIndices != nullptr) {
        glEnable(GL_LINE_SMOOTH);
        if (mStereo) {
            glColor3f(1, 1, 1);
        } else {
            glColor3f(static_cast<float>(mEmbColor.redF()), static_cast<float>(mEmbColor.greenF()), static_cast<float>(mEmbColor.blueF()));
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
            glDrawElements(GL_QUAD_STRIP, mCount[i], GL_UNSIGNED_INT, mEmbIndices[i]);
        }
#endif // GL_GLEXT_PROTOTYPES
#endif
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_LINE_SMOOTH);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // -----------------------
    //   draw geodesic
    // -----------------------
    glLineWidth(mLineWidth);
    if (mLineSmooth == 1) {
        glEnable(GL_LINE_SMOOTH);
    }

    if (mStereo) {
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glColor3f(static_cast<float>(mFGcolor.redF()), static_cast<float>(mFGcolor.greenF()), static_cast<float>(mFGcolor.blueF()));
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

    // -----------------------
    //   draw Sachs
    // -----------------------
    shader->bind();
    if (mStereo) {
        shader->setUniformValue(shader->uniformLocation("colortype"), 0);
    } else {
        shader->setUniformValue(shader->uniformLocation("colortype"), 2);
    }

    shader->setUniformValue("col1",
        static_cast<float>(mParams->opengl_leg1_col1.redF()),
        static_cast<float>(mParams->opengl_leg1_col1.greenF()),
        static_cast<float>(mParams->opengl_leg1_col1.blueF()));

    shader->setUniformValue("col2",
        static_cast<float>(mParams->opengl_leg1_col2.redF()),
        static_cast<float>(mParams->opengl_leg1_col2.greenF()),
        static_cast<float>(mParams->opengl_leg1_col2.blueF()));

    shader->setUniformValue("freq", static_cast<float>(mParams->opengl_leg1_freq));
    shader->setUniformValue("useFog", static_cast<int>(mUseFog));
    shader->setUniformValue("fogFactor", static_cast<float>(-mFogDensity * mFogDensity * 1.442695));
    shader->setUniformValue("eyePos", light_position[0], light_position[1], light_position[2]);

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
            shader->setUniformValue("colortype", 1);
        } else {
            shader->setUniformValue("colortype", 2);
        }

        shader->setUniformValue("col1",
            static_cast<float>(mParams->opengl_leg2_col1.redF()),
            static_cast<float>(mParams->opengl_leg2_col1.greenF()),
            static_cast<float>(mParams->opengl_leg2_col1.blueF()));

        shader->setUniformValue("col2",
            static_cast<float>(mParams->opengl_leg2_col2.redF()),
            static_cast<float>(mParams->opengl_leg2_col2.greenF()),
            static_cast<float>(mParams->opengl_leg2_col2.blueF()));

        shader->setUniformValue("freq", static_cast<float>(mParams->opengl_leg2_freq));
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

    // -----------------------
    //   draw objects
    // -----------------------
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
                font.setPixelSize(static_cast<int>(size));
                //this->renderText((double)cx, (double)cy, (double)cz, QString(mObjects[i]->getText().c_str()), font);
            }
        }
        glDisable(GL_LIGHTING);
    }
}


void OpenGL3dModel::paintGL_stereo() {
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


void OpenGL3dModel::paintGL_axes() {
    mCamera.perspectiveAxes();
    glViewport(0, 0, 100, 100);
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
   // qglColor(textColor);
    //renderText(0.0, 0.0, 1.2, QString("x"));
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
   // qglColor(textColor);
   // renderText(-0.1, 0.1, 1.2, QString("y"));
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
   // qglColor(textColor);
   // renderText(-0.1, -0.1, 1.2, mNameOfZaxis);
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


void OpenGL3dModel::resizeGL(int width, int height) {
    mCamera.setSize(static_cast<int>(width * mDPIFactor[0]), static_cast<int>(height * mDPIFactor[1]));
    update();
}


void OpenGL3dModel::keyPressEvent(QKeyEvent* event) {
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
    update();
}


void OpenGL3dModel::keyReleaseEvent(QKeyEvent* event) {
    mKeyPressed = Qt::Key_No;
    event->ignore();
}


void OpenGL3dModel::mousePressEvent(QMouseEvent * event) {
    mButtonPressed = event->button();
    QPoint cp = event->pos();
    cp.setX(static_cast<int>(cp.x() * mDPIFactor[0]));
    cp.setY(static_cast<int>(cp.y() * mDPIFactor[1]));
    mLastPos = cp;
}


void OpenGL3dModel::mouseReleaseEvent(QMouseEvent * event) {
    mButtonPressed = Qt::NoButton;
    event->accept();
}


void OpenGL3dModel::mouseMoveEvent(QMouseEvent * event) {
    QPoint cp = event->pos();
    cp.setX(static_cast<int>(cp.x() * mDPIFactor[0]));
    cp.setY(static_cast<int>(cp.y() * mDPIFactor[1]));
    QPoint dxy = cp - mLastPos;
    mLastPos   = cp;

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

    update();
}


QString OpenGL3dModel::getVertexShaderCode() {
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


QString OpenGL3dModel::getFragmentShaderCode() {
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
