/**
 * @file    opengl3d_model.h
 * @author  Thomas Mueller
 *
 * @brief  Render engine for the 3d representation of geodesics based on OpenGL.
 *
 * This file is part of GeodesicView.
 */
#ifndef OPENGL3D_MODEL_H
#define OPENGL3D_MODEL_H

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

#include <gdefs.h>
#include <utils/camera.h>
#include <utils/myobject.h>
#include <utils/utilities.h>

#include <extra/m4dObject.h>
#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>

/**
 * @brief The OpenGL3dModel class
 */
class OpenGL3dModel : public QOpenGLWidget
{
    Q_OBJECT

public:
    OpenGL3dModel(struct_params* par, QWidget* parent = nullptr);
    virtual ~OpenGL3dModel();

public:
    void setPoints(m4d::enum_draw_type dtype = m4d::enum_draw_pseudocart, bool needUpdate = true);
    void clearPoints();

    void setSachsAxes(bool needUpdate = true);
    void clearSachsAxes();

    void genEmbed(m4d::Metric* currMetric);
    void clearEmbed();

    void setProjection(enum_projection proj);
    void setFieldOfView(double fov);
    double getFieldOfView();

    void setCameraPos(m4d::vec3 pos);
    m4d::vec3 getCameraPos();
    void setCameraPoi(m4d::vec3 poi);
    m4d::vec3 getCameraPoi();
    void setCameraVup(m4d::vec3 vup);
    m4d::vec3 getCameraVup();
    void setCameraDirs(m4d::vec3 pos, m4d::vec3 poi, m4d::vec3 vup);
    void getCameraDirs(m4d::vec3& pos, m4d::vec3& poi, m4d::vec3& vup);

    void setCameraSphere(double theta, double phi, double dist);
    void getCameraSphere(double& theta, double& phi, double& dist);

    void clearAllObjects();
    void insertObject(MyObject* obj);

    void setFGcolor(QColor col);
    void setBGcolor(QColor col);
    void setEmbColor(QColor col);
    void setColors(QColor fgcol, QColor bgcol, QColor embcol);

    void setLineWidth(int width);
    void setLineSmooth(int smooth);
    void setStyle(enum_draw_style style);

    void setScaling(double sx, double sy, double sz);
    void getScaling(double& sx, double& sy, double& sz);

    void setStereoCam(bool useStereo);
    void setStereoParams(double sep, enum_stereo_glasses type);
    void setStereoType(enum_stereo_type type);

    void setFog(bool useFog);
    void setFogDensity(double density);

    void playAnimRotation(bool play);
    void setAnimRotationParams(double rx, double ry, double rz);
    void getAnimRotationParams(double& rx, double& ry, double& rz);
    void doAnimRotation(double msec, bool local = false);

    void showNumVerts(int num);

    bool saveRGBimage(QString filename);

    void updateParams();
    void reset();

signals:
    void cameraMoved();

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void paintGL_mono();
    virtual void paintGL_stereo();
    virtual void paintGL_axes();
    virtual void resizeGL(int width, int height);

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

    QString getVertexShaderCode();
    QString getFragmentShaderCode();

private:
    struct_params* mParams;
    Camera mCamera;
    double mDPIFactor[2];

    enum_projection mProjection;
    enum_draw_style mDrawStyle;
    bool mStereo;

    QPoint mLastPos;
    int mKeyPressed;
    Qt::KeyboardModifiers mModifiers;
    Qt::MouseButton mButtonPressed;

    QColor mFGcolor;
    QColor mBGcolor;

    GLfloat* mVerts;
    GLfloat* mLambda;
    int mNumVerts;
    int mLineWidth;
    int mLineSmooth;
    int mShowNumVerts;

    GLfloat* mSachsVerts1;
    int mNumSachsVerts1;
    GLfloat* mSachsVerts2;
    int mNumSachsVerts2;
    enum_sachs_legs mSachsLegs;
    double mSachsScale;

    GLfloat* mEmbVerts;
    unsigned int mEmbNumVerts;
    unsigned int mEmbNumElems;
    unsigned int mEmbCounter;
    unsigned int** mEmbIndices;
    GLsizei* mCount;
    QColor mEmbColor;

    std::vector<MyObject*> mObjects;
    bool mWiredObjs;

    double mScaleX;
    double mScaleY;
    double mScaleZ;

    GLfloat light_diffuse[4];
    GLfloat light_position[4];
    GLfloat light_direction[3];

    GLfloat mat_ambient[4];
    GLfloat mat_diffuse[4];
    GLfloat mat_specular[4];

    bool mUseFog;
    double mFogDensity;

    bool mPlayRot;
    double mAnimRotX;
    double mAnimRotY;
    double mAnimRotZ;

    QString mNameOfZaxis;
    GLUquadricObj* mQuadricAxis;
    GLUquadricObj* mQuadricDisk;
    float redLight[3];
    float greenLight[3];
    float blueLight[3];

    bool mGLSLsupported;
    QOpenGLShaderProgram* shader;
};

#endif // OPENGL3D_MODEL_H
