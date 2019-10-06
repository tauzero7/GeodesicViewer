/**
 * @file    camera.h
 * @author  Thomas Mueller
 *
 * @brief  Camera
 *
 * This file is part of GeodesicView.
 */
#ifndef CAMERA_H
#define CAMERA_H

#include <gdefs.h>
#include <iostream>

// must be before gl.h includes
#ifdef _WIN32
#include <windows.h>
#endif

#include <m4dGlobalDefs.h>
#include <utils/quaternions.h>

/**
 * @brief The Camera class
 */
class Camera
{
public:
    Camera();
    virtual ~Camera();

public:
    void setStandardParams();

    void setEyePos(m4d::vec3 pos);
    m4d::vec3 getEyePos();
    void getEyePos(float* pos);

    void setDir(m4d::vec3 dir);
    m4d::vec3 getDir();
    void getDir(float* dir);

    void setPOI(m4d::vec3 center);
    m4d::vec3 getPOI();

    void setVup(m4d::vec3 vup);
    m4d::vec3 getVup();

    m4d::vec3 getRight();

    double getDistance();

    void setFovY(double fovy);
    double getFovY();

    double getAspect();

    void setIntrinsic(double fovy, double nearval, double farval);
    void getIntrinsic(double& fovy, double& nearval, double& farval);

    void setSize(int width, int height);
    void getSize(int& width, int& height);

    void fixRotAroundVup(double angle);
    void fixRotAroundRight(double angle);
    void fixRotAroundDir(double angle);

    void fixRotAroundX(double angle);
    void fixRotAroundY(double angle);
    void fixRotAroundZ(double angle);

    void getSphericalEyePos(double& theta, double& phi, double& dist);

    /*! Move camera along sphere.
     * \param theta : angle theta.
     * \param phi : angle phi.
     * \param dist : radius of sphere.
     */
    void moveOnSphere(double theta, double phi, double dist);

    void rotate(double dazi, double dlat, bool use_local_z = true);

    void setSpecialPos(enum_coord_axis pos);
    void setPosRFrame(m4d::vec3 pos, m4d::vec3 dir, m4d::vec3 up);

    void lookAt();
    void lookAtCenter();
    void lookAtModelView();
    void lookAtCenterModelView();

    void setStereoParams(double sep, enum_stereo_glasses type = enum_stereo_red_cyan);
    void getStereoParams(double& sep, enum_stereo_glasses& type);

    void setStereoType(enum_stereo_type type);
    void getStereoType(enum_stereo_type& type);

    void lookAtMV_Left();
    void lookAtMV_Right();

    void perspective();
    void perspectiveAxes();
    void orthographic();
    void viewport();

    void print(FILE* ptr = stderr);

protected:
    m4d::vec3 mPos;
    m4d::vec3 mDir;
    m4d::vec3 mVup;

    GLdouble mZnear;
    GLdouble mZfar;
    GLdouble mAspect;
    int mWidth;
    int mHeight;
    GLdouble mFovY;

    double mEyeSep;
    enum_stereo_glasses mStereoGlasses;
    enum_stereo_type mStereoType;

    m4d::vec3 mRight;
    m4d::vec3 mPOI;
    Quaternion mQuat;
};

#endif // CAMERA_H
