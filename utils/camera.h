// --------------------------------------------------------------------------------
/*
    camera.h

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

/*!  \class  Camera
     \brief  Camera handling.

*/
// --------------------------------------------------------------------------------

#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <gdefs.h>

//must be before gl.h includes
#ifdef _WIN32
#include <windows.h>
#endif

#ifndef __APPLE__
//#include <GL/gl.h>
//#include <GL/glu.h>
#else
//#include <gl.h>
//#include <glu.h>
#endif

#include <m4dGlobalDefs.h>
#include <utils/quaternions.h>


// ---------------------------------------------------
//     class definition:   Camera
// ---------------------------------------------------
class Camera {
public:
    Camera();
    virtual ~Camera();

// --------- public methods -----------
public:
    void   setStandardParams();

    void        setEyePos(m4d::vec3 pos);
    m4d::vec3   getEyePos();
    void        getEyePos(float* pos);

    void        setDir(m4d::vec3 dir);
    m4d::vec3   getDir();
    void        getDir(float* dir);

    void        setPOI(m4d::vec3 center);
    m4d::vec3   getPOI();
    void        getPOI(float* poi);

    void        setVup(m4d::vec3 vup);
    m4d::vec3   getVup();
    void        getVup(float* vup);

    m4d::vec3   getRight();
    void        getRight(float* right);

    void        setFovY(double fovy);
    double      getFovY();

    double      getAspect();

    void        setIntrinsic(double fovy, double nearval, double farval);
    void        getIntrinsic(double &fovy, double &nearval, double &farval);

    void        setSizeAndAspect(int width, int  height);
    void        getSize(int &width, int &height);

    void        fixRotAroundVup(double angle);
    void        fixRotAroundRight(double angle);
    void        fixRotAroundDir(double angle);

    void        fixRotAroundX(double angle);
    void        fixRotAroundY(double angle);
    void        fixRotAroundZ(double angle);

    void        getSphericalEyePos(double &theta, double &phi, double &dist);
    void        moveOnSphere(double theta, double phi, double dist);

    void        lookAt();
    void        lookAtCenter();
    void        lookAtModelView();
    void        lookAtCenterModelView();

    void        setStereoParams(double sep, enum_stereo_glasses type = enum_stereo_red_cyan);
    void        getStereoParams(double &sep, enum_stereo_glasses &type);

    void        setStereoType(enum_stereo_type type);
    void        getStereoType(enum_stereo_type &type);

    void        lookAtMV_Left();
    void        lookAtMV_Right();

    void        perspective();
    void        perspectiveAxes();
    void        orthographic();
    void        viewport();

    void        print(FILE* ptr = stderr);

// --------- protected attributes -----------
protected:
    m4d::vec3    mPos;
    m4d::vec3    mDir;
    m4d::vec3    mVup;

    GLdouble     mZnear;
    GLdouble     mZfar;
    GLdouble     mAspect;
    int          mWidth;
    int          mHeight;
    GLdouble     mFovY;

    double                mEyeSep;
    enum_stereo_glasses   mStereoGlasses;
    enum_stereo_type      mStereoType;

    m4d::vec3    mRight;
    m4d::vec3    mPOI;
};
#endif
