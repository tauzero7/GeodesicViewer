// --------------------------------------------------------------------------------
/*
    object.cpp

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

#include "myobject.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

/*! Standard object constructor.
 *  \param prepare : do prepare.
 */
MyObject::MyObject(bool prepare) {
    mObject.type = enum_object_undefined;
    mObject.dim  = enum_object_dim_0d;
    mObject.color[0] = 1.0;
    mObject.color[1] = 1.0;
    mObject.color[2] = 1.0;
    mObject.color[3] = 1.0;

    mObject.text = std::string();
    for (int i = 0; i < NUM_OBJECT_MAX_VALUES; i++) {
        mObject.val[i] = 0.0;
    }
    mObject.numvals = 0;
    mObject.lighted = false;
    mObject.style = GLU_LINE;

    myQuad = NULL;
    mDisplList = 0;
    mPrepare = prepare;
}

/*! Object constructor with colors.
 *  \param  red   : red color value.
 *  \param  green : green color value.
 *  \param  blue  : blue color value.
 *  \param  alpha : alpha value.
 */
MyObject::MyObject(float red, float green, float blue, float alpha) {
    mObject.type = enum_object_undefined;
    mObject.dim  = enum_object_dim_0d;
    mObject.color[0] = red;
    mObject.color[1] = green;
    mObject.color[2] = blue;
    mObject.color[3] = alpha;

    mObject.text = std::string();
    for (int i = 0; i < NUM_OBJECT_MAX_VALUES; i++) {
        mObject.val[i] = 0.0;
    }
    mObject.numvals = 0;
    mObject.lighted = false;
    mObject.style = GLU_LINE;
    myQuad = NULL;

    mDisplList = 0;
    mPrepare = true;
}

/*! Copy constructor.
 *  \param obj : reference to MyObject.
 */
MyObject::MyObject(const MyObject &obj) {
    mObject    = obj.mObject;
    myQuad     = obj.myQuad;
    mDisplList = obj.mDisplList;

    deleteQuadric();
    if (mDisplList != 0) {
        glDeleteLists(mDisplList, 1);
    }
    mDisplList = 0;

    mPrepare = false;
    create_object(mObject.type, mObject.val, mObject.text);
}

/*! Standard destructor.
 */
MyObject::~MyObject() {
    deleteQuadric();
    if (mDisplList != 0) {
        glDeleteLists(mDisplList, 1);
    }
    mDisplList = 0;
}

/*! Get type of object.
 *  \return  enum_object_type : type of object.
 */
enum_object_type
MyObject::getObjectType() {
    return mObject.type;
}

/*! Get dimension of object.
 *  \return  enum_object_dim : object dimension: 2d/3d.
 */
enum_object_dim
MyObject::getObjectDim() {
    return mObject.dim;
}

/*! Get object struct.
 *  \param  obj : reference to struct_obj.
 */
void
MyObject::getObject(struct_obj &obj) {
    obj.type  = mObject.type;
    obj.dim   = mObject.dim;
    for (int i = 0; i < NUM_OBJECT_MAX_VALUES; i++) {
        obj.val[i] = mObject.val[i];
    }
    obj.text  = mObject.text;
}

/*! Get value.
 * \param num : number of value.
 * \param val : reference to value.
 * \return true : value exists.
 */
bool
MyObject::getValue(int num, float &val) {
    if (num < 0 || num >= mObject.numvals) {
        return false;
    }
    val = mObject.val[num];
    return true;
}

/*! Get text.
 * \return text.
 */
std::string
MyObject::getText() {
    return mObject.text;
}

/*! Set color of object.
 *  \param  red   : red color value.
 *  \param  green : green color value.
 *  \param  blue  : blue color value.
 *  \param  alpha : alpha value.
 */
void
MyObject::setColor(float red, float green, float blue, float alpha) {
    mObject.color[0] = clamp_float(red, 0.0, 1.0);
    mObject.color[1] = clamp_float(green, 0.0, 1.0);
    mObject.color[2] = clamp_float(blue, 0.0, 1.0);
    mObject.color[3] = clamp_float(alpha, 0.0, 1.0);
}

/*! Get color of object.
 *  \param  red   : reference to red color value.
 *  \param  green : reference to green color value.
 *  \param  blue  : reference to blue color value.
 */
void
MyObject::getColor(float &red, float &green, float &blue) {
    red   = mObject.color[0];
    green = mObject.color[1];
    blue  = mObject.color[2];
}

/*! Get color of object.
 *  \param  red   : reference to red color value.
 *  \param  green : reference to green color value.
 *  \param  blue  : reference to blue color value.
 *  \param  alpha : reference to alpha value.
 */
void
MyObject::getColor(float &red, float &green, float &blue, float &alpha) {
    red   = mObject.color[0];
    green = mObject.color[1];
    blue  = mObject.color[2];
    alpha = mObject.color[3];
}

/*! Is object lit by a light.
 *  \param  light  : pointer to light array values.
 *  \param  stereo : draw object in stereo mode.
 *  \return true  : object is lit ny light source.
 *  \return false : no light.
 */
bool
MyObject::withLight(float light[], bool stereo) {
    if (mObject.lighted) {
        if (!stereo) {
            light[0] = mObject.color[0];
            light[1] = mObject.color[1];
            light[2] = mObject.color[2];
            light[3] = mObject.color[3];
        } else {
            light[0] = light[1] = light[2] = light[3] = 1.0;
        }
        return true;
    }
    return false;
}

/*! Create object.
 *  \param type : object type.
 *  \param val  : pointer to values for the object creation.
 *  \param text : text.
 */
void
MyObject::create_object(enum_object_type  type, float* val, std::string text) {
    switch (type) {
        case enum_object_undefined:
            break;
        case enum_object_text2d:
            create_2d_text(val[0], val[1], text, val[2]);
            break;
        case enum_object_text3d:
            create_3d_text(val[0], val[1], val[2], text, val[3]);
            break;
        case enum_object_sphere2d:
            create_2d_sphere(val[0], val[1], val[2], val[3], val[4]);
            break;
        case enum_object_sphere3d:
            create_3d_sphere(val[0], val[1], val[2], val[3], val[4], val[5]);
            break;
        case enum_object_box2d:
            create_2d_box(val[0], val[1], val[2], val[3], val[4]);
            break;
        case enum_object_box3d:
            create_3d_box(val[0], val[1], val[2], val[3], val[4], val[5], val[6]);
            break;
        case enum_object_line2d:
            create_2d_line(val[0], val[1], val[2], val[3], val[4]);
            break;
        case enum_object_line3d:
            create_3d_line(val[0], val[1], val[2], val[3], val[4], val[5], val[6]);
            break;
        case enum_object_quad2d:
            create_2d_quad(val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8]);
            break;
        case enum_object_plane3d:
            create_3d_plane(val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8]);
            break;
        case enum_object_disk2d:
            create_2d_disk(val[0], val[1], val[2], val[3]);
            break;
        case enum_object_disk3d:
            create_3d_disk(val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9]);
            break;
        case enum_object_cylinder3d:
            create_3d_cylinder(val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9]);
            break;
        case enum_object_torus3d:
            create_3d_torus(val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9]);
            break;
        case enum_object_tube3d:
            create_3d_tube(val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9], val[10], val[11], val[12], val[13]);
            break;
    }
}

/*! Create 2d text.
 *  \param cx : x-coordinate of first character.
 *  \param cy : y-coordinate of first character.
 *  \param text : text.
 *  \param size : size of text.
 */
void
MyObject::create_2d_text(float cx, float cy, std::string text, float size) {
    mObject.type    = enum_object_text2d;
    mObject.dim     = enum_object_dim_2d;
    mObject.val[0]  = cx;
    mObject.val[1]  = cy;
    mObject.val[2]  = size;
    mObject.text    = text.c_str();
    mObject.numvals = 3;
    mObject.lighted = false;
}

/*! Create 2d text.
 *  \param cx : x-coordinate of first character.
 *  \param cy : y-coordinate of first character.
 *  \param cz : z-coordinate of first character.
 *  \param text : text.
 *  \param size : size of text.
 */
void
MyObject::create_3d_text(float cx, float cy, float cz, std::string text, float size) {
    mObject.type    = enum_object_text3d;
    mObject.dim     = enum_object_dim_3d;
    mObject.val[0]  = cx;
    mObject.val[1]  = cy;
    mObject.val[2]  = cz;
    mObject.val[3]  = size;
    mObject.text    = text.c_str();
    mObject.numvals = 4;
    mObject.lighted = false;
}


/*! Create 2d sphere.
 *  \param cx : x-coordinate of center.
 *  \param cy : y-coordinate of center.
 *  \param radius : radius of circle.
 *  \param num_points : number of points of circle.
 *  \param width : width.
 */
void
MyObject::create_2d_sphere(float cx, float cy, float radius, float num_points, float width) {
    deleteQuadric();

    mObject.type = enum_object_sphere2d;
    mObject.dim  = enum_object_dim_2d;
    mObject.val[0] = cx;
    mObject.val[1] = cy;
    mObject.val[2] = radius;
    mObject.val[3] = num_points;
    mObject.val[4] = width;
    mObject.numvals = 5;
    mObject.lighted = false;
}

/*! Create 3d sphere.
 *  \param cx : x-coordinate of center.
 *  \param cy : y-coordinate of center.
 *  \param cz : z-coordinate of center.
 *  \param radius : radius of isphere.
 *  \param slices : number of slices.
 *  \param stacks : number of stacks.
 */
void
MyObject::create_3d_sphere(float cx, float cy, float cz, float radius, float slices, float stacks) {
    mObject.type = enum_object_sphere3d;
    mObject.dim  = enum_object_dim_3d;
    mObject.val[0] = cx;
    mObject.val[1] = cy;
    mObject.val[2] = cz;
    mObject.val[3] = radius;
    mObject.val[4] = slices;
    mObject.val[5] = stacks;
    mObject.numvals = 6;
    mObject.lighted = true;
    mObject.style = GLU_FILL;
    createQuadric();
}

/*! Create 2d box.
 *  \param cx : x-coordinate of center.
 *  \param cy : y-coordinate of center.
 *  \param sizeX : length of the x-side.
 *  \param sizeY : length of the y-side.
 *  \param width : width of lines.
 */
void
MyObject::create_2d_box(float cx, float cy, float sizeX, float sizeY, float width) {
    deleteQuadric();

    mObject.type = enum_object_box2d;
    mObject.dim  = enum_object_dim_2d;
    mObject.val[0] = cx;
    mObject.val[1] = cy;
    mObject.val[2] = sizeX;
    mObject.val[3] = sizeY;
    mObject.val[4] = width;
    mObject.numvals = 5;
    mObject.lighted = false;
}

/*! Create 3d box.
 *  \param cx : x-coordinate of center.
 *  \param cy : y-coordinate of center.
 *  \param cz : z-coordinate of center.
 *  \param sizeX : length of the x-side.
 *  \param sizeY : length of the y-side.
 *  \param sizeZ : length of the z-side.
 *  \param width : width of lines.
 */
void
MyObject::create_3d_box(float cx, float cy, float cz, float sizeX, float sizeY, float sizeZ, float width) {
    deleteQuadric();

    mObject.type = enum_object_box3d;
    mObject.dim  = enum_object_dim_3d;
    mObject.val[0] = cx;
    mObject.val[1] = cy;
    mObject.val[2] = cz;
    mObject.val[3] = sizeX;
    mObject.val[4] = sizeY;
    mObject.val[5] = sizeZ;
    mObject.val[6] = width;
    mObject.numvals = 7;
    mObject.lighted = false;
}

/*! Create 2d line.
 *  \param lx : x-coordinate of point 1.
 *  \param ly : y-coordinate of point 1.
 *  \param ux : x-coordinate of point 2.
 *  \param uy : y-coordinate of point 2.
 *  \param width : line width.
 */
void
MyObject::create_2d_line(float lx, float ly, float ux, float uy, float width) {
    deleteQuadric();

    mObject.type = enum_object_line2d;
    mObject.dim  = enum_object_dim_2d;
    mObject.val[0] = lx;
    mObject.val[1] = ly;
    mObject.val[2] = ux;
    mObject.val[3] = uy;
    mObject.val[4] = width;
    mObject.numvals = 5;
    mObject.lighted = false;
}

/*! Create 3d line.
 *  \param lx : x-coordinate of point 1.
 *  \param ly : y-coordinate of point 1.
 *  \param lz : z-coordinate of point 1.
 *  \param ux : x-coordinate of point 2.
 *  \param uy : y-coordinate of point 2.
 *  \param uz : z-coordinate of point 2.
 *  \param width : line width.
 */
void
MyObject::create_3d_line(float lx, float ly, float lz, float ux, float uy, float uz, float width) {
    deleteQuadric();

    mObject.type = enum_object_line3d;
    mObject.dim  = enum_object_dim_3d;
    mObject.val[0] = lx;
    mObject.val[1] = ly;
    mObject.val[2] = lz;
    mObject.val[3] = ux;
    mObject.val[4] = uy;
    mObject.val[5] = uz;
    mObject.val[6] = width;
    mObject.numvals = 7;
    mObject.lighted = false;
}

/*! Create 2d quad.
 *  \param v1x :  x-coordinate of vertex 1.
 *  \param v1y :  y-coordinate of vertex 1.
 *  \param v2x :  x-coordinate of vertex 2.
 *  \param v2y :  y-coordinate of vertex 2.
 *  \param v3x :  x-coordinate of vertex 3.
 *  \param v3y :  y-coordinate of vertex 3.
 *  \param v4x :  x-coordinate of vertex 4.
 *  \param v4y :  y-coordinate of vertex 4.
 *  \param width : line width.
 */
void
MyObject::create_2d_quad(float v1x, float v1y, float v2x, float v2y, float v3x, float v3y, float v4x, float v4y, float width) {
    deleteQuadric();

    mObject.type = enum_object_quad2d;
    mObject.dim  = enum_object_dim_2d;
    mObject.val[0] = v1x;
    mObject.val[1] = v1y;
    mObject.val[2] = v2x;
    mObject.val[3] = v2y;
    mObject.val[4] = v3x;
    mObject.val[5] = v3y;
    mObject.val[6] = v4x;
    mObject.val[7] = v4y;
    mObject.val[8] = width;
    mObject.numvals = 9;
    mObject.lighted = false;
}


/*! Create 3d plane.
 *  \param v1x : x-coordinate of vertex 1.
 *  \param v1y : y-coordinate of vertex 1.
 *  \param v1z : z-coordinate of vertex 1.
 *  \param v2x : x-coordinate of vertex 2.
 *  \param v2y : y-coordinate of vertex 2.
 *  \param v2z : z-coordinate of vertex 2.
 *  \param v3x : x-coordinate of vertex 3.
 *  \param v3y : y-coordinate of vertex 3.
 *  \param v3z : z-coordinate of vertex 3.
 */
void
MyObject::create_3d_plane(float v1x, float v1y, float v1z,
                          float v2x, float v2y, float v2z,
                          float v3x, float v3y, float v3z) {
    deleteQuadric();

    mObject.type = enum_object_plane3d;
    mObject.dim  = enum_object_dim_3d;

    mObject.val[0] = v1x;
    mObject.val[1] = v1y;
    mObject.val[2] = v1z;
    mObject.val[3] = v2x;
    mObject.val[4] = v2y;
    mObject.val[5] = v2z;
    mObject.val[6] = v3x;
    mObject.val[7] = v3y;
    mObject.val[8] = v3z;
    mObject.numvals = 9;
    mObject.lighted = false;
    mObject.style = GLU_FILL;
}


/*! Create 2d disk.
 *  \param cx : x-coordinate of center.
 *  \param cy : y-coordinate of center.
 *  \param radius : radius of the disk
 *  \param fans : number of triangle fans.
 */
void
MyObject::create_2d_disk(float cx, float cy, float radius, float fans) {
    deleteQuadric();

    mObject.type = enum_object_disk2d;
    mObject.dim  = enum_object_dim_2d;

    mObject.val[0] = cx;
    mObject.val[1] = cy;
    mObject.val[2] = radius;
    mObject.val[3] = fans;
    mObject.numvals = 4;
    mObject.lighted = false;
    mObject.style = GLU_FILL;
}

/*! Create 3d disk.
 *  \param cx : x-coordinate of center.
 *  \param cy : y-coordinate of center.
 *  \param cz : z-coordinate of center.
 *  \param dx : x-direction.
 *  \param dy : y-direction.
 *  \param dz : z-direction.
 *  \param iRadius : inner radius of the disk
 *  \param oRadius : outer radius of the disk.
 *  \param slices : number of slices.
 *  \param loops : number of loops.
 */
void
MyObject::create_3d_disk(float cx, float cy, float cz,
                         float dx, float dy, float dz,
                         float iRadius, float oRadius,
                         float slices, float loops) {
    //std::cerr << "disk created" << std::endl;
    mObject.type = enum_object_disk3d;
    mObject.dim  = enum_object_dim_3d;

    mObject.val[0] = cx;
    mObject.val[1] = cy;
    mObject.val[2] = cz;
    mObject.val[3] = dx;
    mObject.val[4] = dy;
    mObject.val[5] = dz;
    mObject.val[6] = iRadius;
    mObject.val[7] = oRadius;
    mObject.val[8] = slices;
    mObject.val[9] = loops;
    mObject.numvals = 10;
    mObject.lighted = false;
    mObject.style = GLU_FILL;
    createQuadric();
}

/*! Create 3d cylinder.
 *  \param bx : x-coordinate of base center.
 *  \param by : y-coordinate of base center.
 *  \param bz : z-coordinate of base center.
 *  \param tx : x-coordinate of top center.
 *  \param ty : y-coordinate of top center.
 *  \param tz : z-coordinate of top center.
 *  \param baseRadius : base radius.
 *  \param topRadius : top radius.
 *  \param slices : number of slices.
 *  \param stacks : number of stacks.
 */
void
MyObject::create_3d_cylinder(float bx, float by, float bz, float tx, float ty, float tz,
                             float baseRadius, float topRadius, float slices, float stacks) {
    mObject.type = enum_object_cylinder3d;
    mObject.dim  = enum_object_dim_3d;

    mObject.val[0] = bx;
    mObject.val[1] = by;
    mObject.val[2] = bz;
    mObject.val[3] = tx;
    mObject.val[4] = ty;
    mObject.val[5] = tz;
    mObject.val[6] = baseRadius;
    mObject.val[7] = topRadius;
    mObject.val[8] = slices;
    mObject.val[9] = stacks;
    mObject.numvals = 10;
    mObject.lighted = true;
    mObject.style = GLU_FILL;
    createQuadric();
}

/*!
 */
void
MyObject::torus(const float R, const float r, const float alpha, const float beta,
                float &x, float &y, float &z, float &dx, float &dy, float &dz) {
    x = (R + r * cos(beta)) * cos(alpha);
    y = (R + r * cos(beta)) * sin(alpha);
    z = r * sin(beta);

    dx = cos(beta) * cos(alpha);
    dy = cos(beta) * sin(alpha);
    dz = sin(beta);
}

/*!
 */
void
MyObject::create_3d_torus(float cx, float cy, float cz, float nt, float np,
                          float outerRadius, float innerRadius, float maxPhi,
                          float slices, float stacks) {
    mObject.type = enum_object_torus3d;
    mObject.dim  = enum_object_dim_3d;

    mObject.val[0] = cx;
    mObject.val[1] = cy;
    mObject.val[2] = cz;
    mObject.val[3] = nt;
    mObject.val[4] = np;
    mObject.val[5] = outerRadius;
    mObject.val[6] = innerRadius;
    mObject.val[7] = maxPhi;
    mObject.val[8] = slices;
    mObject.val[9] = stacks;
    mObject.numvals = 10;
    mObject.lighted = true;
    mObject.style = GL_FILL;

    if (maxPhi < 0.0) {
        mObject.val[7] = 0.0;
    }
    if (maxPhi > 360.0) {
        mObject.val[7] = 360.0;
    }

    // if (mPrepare) return;

    if (mDisplList != 0) {
        glDeleteLists(mDisplList, 1);
    }

    mDisplList = glGenLists(1);
    glNewList(mDisplList, GL_COMPILE);
    float x, y, z, dx, dy, dz, alpha, beta;
    float alphaStep = 2.0 * M_PI / slices;
    float betaStep  = 2.0 * M_PI / stacks;

    float mp = mObject.val[7] * DEG_TO_RAD;
    for (alpha = 0.0; alpha < mp; alpha += alphaStep) {
        glBegin(GL_QUAD_STRIP);
        beta = 0.0;
        while (beta < 2.0 * M_PI) {
            torus(outerRadius, innerRadius, alpha, beta, x, y, z, dx, dy, dz);
            glNormal3f(dx, dy, dz);
            glVertex3f(x, y, z);
            torus(outerRadius, innerRadius, alpha + alphaStep, beta, x, y, z, dx, dy, dz);
            glNormal3f(dx, dy, dz);
            glVertex3f(x, y, z);
            beta += betaStep;
        }
        torus(outerRadius, innerRadius, alpha, beta, x, y, z, dx, dy, dz);
        glNormal3f(dx, dy, dz);
        glVertex3f(x, y, z);
        torus(outerRadius, innerRadius, alpha + alphaStep, 0.0, x, y, z, dx, dy, dz);
        glNormal3f(dx, dy, dz);
        glVertex3f(x, y, z);
        glEnd();
    }
    glEndList();
}

/*! Create 3d tube.
 */
void
MyObject::create_3d_tube(float bx, float by, float bz,  float e1x, float e1y, float e1z,
                         float e2x, float e2y, float e2z,  float e3x, float e3y, float e3z,
                         float slices, float stacks) {
    mObject.type = enum_object_tube3d;
    mObject.dim  = enum_object_dim_3d;

    mObject.val[ 0] = bx;
    mObject.val[ 1] = by;
    mObject.val[ 2] = bz;
    mObject.val[ 3] = e1x;
    mObject.val[ 4] = e1y;
    mObject.val[ 5] = e1z;
    mObject.val[ 6] = e2x;
    mObject.val[ 7] = e2y;
    mObject.val[ 8] = e2z;
    mObject.val[ 9] = e3x;
    mObject.val[10] = e3y;
    mObject.val[11] = e3z;
    mObject.val[12] = slices;
    mObject.val[13] = stacks;
    mObject.numvals = 14;
    mObject.lighted = true;
    mObject.style = GL_FILL;

    if (mDisplList != 0) {
        glDeleteLists(mDisplList, 1);
    }

    mDisplList = glGenLists(1);
    glNewList(mDisplList, GL_COMPILE);

    float x, y, z, dx, dy, dz, alpha;
    float alphaStep = 2.0 * M_PI / slices;
    float lStep     = 1.0 / stacks;

    for (float l = 0.0; l < 1.0; l += lStep) {
        glBegin(GL_QUAD_STRIP);
        for (alpha = 0.0; alpha < 2.0 * M_PI; alpha += alphaStep) {
            dx = e1x * cos(alpha) + e2x * sin(alpha);
            dy = e1y * cos(alpha) + e2y * sin(alpha);
            dz = e1z * cos(alpha) + e2z * sin(alpha);

            x = bx + dx + l * e3x;
            y = by + dy + l * e3y;
            z = bz + dz + l * e3z;

            glNormal3f(-dx, -dy, -dz);
            glVertex3f(x, y, z);

            x = bx + dx + (l + lStep) * e3x;
            y = by + dy + (l + lStep) * e3y;
            z = bz + dz + (l + lStep) * e3z;

            glNormal3f(-dx, -dy, -dz);
            glVertex3f(x, y, z);
        }
        alpha = 0.0;
        dx = e1x * cos(alpha) + e2x * sin(alpha);
        dy = e1y * cos(alpha) + e2y * sin(alpha);
        dz = e1z * cos(alpha) + e2z * sin(alpha);

        x = bx + dx + l * e3x;
        y = by + dy + l * e3y;
        z = bz + dz + l * e3z;

        glNormal3f(-dx, -dy, -dz);
        glVertex3f(x, y, z);

        x = bx + dx + (l + lStep) * e3x;
        y = by + dy + (l + lStep) * e3y;
        z = bz + dz + (l + lStep) * e3z;

        glNormal3f(-dx, -dy, -dz);
        glVertex3f(x, y, z);
        glEnd();
    }
    glEndList();
}

/*! Draw object.
 *  \param  stereo :  stereo mode(true), mono mode(false).
 */
bool
MyObject::drawObject(bool stereo) {
    if (!stereo) {
        glColor4fv(mObject.color);
    } else {
        glColor4fv(color_white);
    }

    float line_width;
    switch (mObject.type) {
        case enum_object_undefined:
            break;
        case enum_object_text2d:
        case enum_object_text3d: {
            return false;
            break;
        }
        case enum_object_sphere2d: {
            glGetFloatv(GL_LINE_WIDTH, &line_width);
            glLineWidth(mObject.val[4]);
            double cx = mObject.val[0];
            double cy = mObject.val[1];
            double radius = mObject.val[2];
            double numPoints = mObject.val[3];
            double angleStep = 2.0 * M_PI / numPoints;
            glBegin(GL_LINE_LOOP);
            for (float angle = 0.0; angle < 2.0 * M_PI; angle += angleStep) {
                glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
            }
            glEnd();
            glLineWidth(line_width);
            break;
        }
        case enum_object_sphere3d: {
            glPushMatrix();
            glTranslatef(mObject.val[0], mObject.val[1], mObject.val[2]);
            gluSphere(myQuad, mObject.val[3], GLuint(mObject.val[4]), GLuint(mObject.val[5]));
            glPopMatrix();
            break;
        }
        case enum_object_box2d: {
            glGetFloatv(GL_LINE_WIDTH, &line_width);
            glLineWidth(mObject.val[4]);
            glBegin(GL_LINE_LOOP);
            glVertex2f(mObject.val[0] - 0.5 * mObject.val[2], mObject.val[1] - 0.5 * mObject.val[3]);
            glVertex2f(mObject.val[0] + 0.5 * mObject.val[2], mObject.val[1] - 0.5 * mObject.val[3]);
            glVertex2f(mObject.val[0] + 0.5 * mObject.val[2], mObject.val[1] + 0.5 * mObject.val[3]);
            glVertex2f(mObject.val[0] - 0.5 * mObject.val[2], mObject.val[1] + 0.5 * mObject.val[3]);
            glEnd();
            glLineWidth(line_width);
            break;
        }
        case enum_object_box3d: {
            glPushMatrix();
            glTranslatef(mObject.val[0], mObject.val[1], mObject.val[2]);

            glGetFloatv(GL_LINE_WIDTH, &line_width);
            glLineWidth(mObject.val[6]);
            glEnable(GL_LINE_SMOOTH);
            glBegin(GL_LINE_LOOP);
            glVertex3f(-0.5 * mObject.val[3], -0.5 * mObject.val[4], -0.5 * mObject.val[5]);
            glVertex3f(0.5 * mObject.val[3], -0.5 * mObject.val[4], -0.5 * mObject.val[5]);
            glVertex3f(0.5 * mObject.val[3],  0.5 * mObject.val[4], -0.5 * mObject.val[5]);
            glVertex3f(-0.5 * mObject.val[3],  0.5 * mObject.val[4], -0.5 * mObject.val[5]);
            glEnd();

            glBegin(GL_LINE_LOOP);
            glVertex3f(-0.5 * mObject.val[3], -0.5 * mObject.val[4],  0.5 * mObject.val[5]);
            glVertex3f(0.5 * mObject.val[3], -0.5 * mObject.val[4],  0.5 * mObject.val[5]);
            glVertex3f(0.5 * mObject.val[3],  0.5 * mObject.val[4],  0.5 * mObject.val[5]);
            glVertex3f(-0.5 * mObject.val[3],  0.5 * mObject.val[4],  0.5 * mObject.val[5]);
            glEnd();

            glBegin(GL_LINES);
            glVertex3f(-0.5 * mObject.val[3], -0.5 * mObject.val[4], -0.5 * mObject.val[5]);
            glVertex3f(-0.5 * mObject.val[3], -0.5 * mObject.val[4],  0.5 * mObject.val[5]);
            glEnd();

            glBegin(GL_LINES);
            glVertex3f(0.5 * mObject.val[3], -0.5 * mObject.val[4], -0.5 * mObject.val[5]);
            glVertex3f(0.5 * mObject.val[3], -0.5 * mObject.val[4],  0.5 * mObject.val[5]);
            glEnd();

            glBegin(GL_LINES);
            glVertex3f(0.5 * mObject.val[3],  0.5 * mObject.val[4], -0.5 * mObject.val[5]);
            glVertex3f(0.5 * mObject.val[3],  0.5 * mObject.val[4],  0.5 * mObject.val[5]);
            glEnd();

            glBegin(GL_LINES);
            glVertex3f(-0.5 * mObject.val[3],  0.5 * mObject.val[4], -0.5 * mObject.val[5]);
            glVertex3f(-0.5 * mObject.val[3],  0.5 * mObject.val[4],  0.5 * mObject.val[5]);
            glEnd();
            glDisable(GL_LINE_SMOOTH);
            glLineWidth(line_width);
            glPopMatrix();
            break;
        }
        case enum_object_line2d: {
            glGetFloatv(GL_LINE_WIDTH, &line_width);
            glLineWidth(mObject.val[4]);
            glEnable(GL_LINE_SMOOTH);
            glBegin(GL_LINES);
            glVertex2f(mObject.val[0], mObject.val[1]);
            glVertex2f(mObject.val[2], mObject.val[3]);
            glEnd();
            glDisable(GL_LINE_SMOOTH);
            glLineWidth(line_width);
            break;
        }
        case enum_object_line3d: {
            glGetFloatv(GL_LINE_WIDTH, &line_width);
            glLineWidth(mObject.val[6]);
            glEnable(GL_LINE_SMOOTH);
            glBegin(GL_LINES);
            glVertex3f(mObject.val[0], mObject.val[1], mObject.val[2]);
            glVertex3f(mObject.val[3], mObject.val[4], mObject.val[5]);
            glEnd();
            glDisable(GL_LINE_SMOOTH);
            glLineWidth(line_width);
            break;
        }
        case enum_object_quad2d: {
            glGetFloatv(GL_LINE_WIDTH, &line_width);
            if (mObject.val[8] > 0.0) {
                glLineWidth(mObject.val[8]);
                glEnable(GL_LINE_SMOOTH);
                glBegin(GL_LINE_LOOP);
            } else {
                glBegin(GL_QUADS);
            }

            glVertex2f(mObject.val[0], mObject.val[1]);
            glVertex2f(mObject.val[2], mObject.val[3]);
            glVertex2f(mObject.val[4], mObject.val[5]);
            glVertex2f(mObject.val[6], mObject.val[7]);
            glEnd();
            glDisable(GL_LINE_SMOOTH);
            glLineWidth(line_width);
            break;
        }
        case enum_object_plane3d: {
            glBegin(GL_QUADS);
            glVertex3f(mObject.val[0], mObject.val[1], mObject.val[2]);
            glVertex3f(mObject.val[3], mObject.val[4], mObject.val[5]);
            glVertex3f(mObject.val[3] + mObject.val[6] - mObject.val[0],
                       mObject.val[4] + mObject.val[7] - mObject.val[1],
                       mObject.val[5] + mObject.val[8] - mObject.val[2]);
            glVertex3f(mObject.val[6], mObject.val[7], mObject.val[8]);
            glEnd();
            break;
        }
        case enum_object_disk2d: {
            float cx = mObject.val[0];
            float cy = mObject.val[1];
            float r  = mObject.val[2];
            float step = 2.0 * M_PI / mObject.val[3];
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (float angle = 0.0; angle < 2.0 * M_PI; angle += step) {
                glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
            }
            glVertex2f(cx + r, cy);
            glEnd();
            break;
        }
        case enum_object_disk3d: {
            m4d::vec3 dir, right;
            m4d::vec3 zup(0.0, 0.0, 1.0);
            double angle = 0.0;

            dir = m4d::vec3(mObject.val[3], mObject.val[4], mObject.val[5]);
            if (dir.isZero()) {
                break;
            }
            dir.normalize();

            right = (zup ^ dir);
            if (right.isZero()) {
                angle = 0.0;
            } else {
                right.normalize();
                angle = acos(zup | dir);
            }

            glPushMatrix();
            glTranslatef(mObject.val[0], mObject.val[1], mObject.val[2]);
            glRotatef(angle * RAD_TO_DEG, right[0], right[1], right[2]);
            gluDisk(myQuad, mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9]);
            glPopMatrix();
            break;
        }
        case enum_object_cylinder3d: {
            m4d::vec3 dir, right;
            m4d::vec3 zup(0.0, 0.0, 1.0);
            double angle = 0.0;

            dir = m4d::vec3(mObject.val[3] - mObject.val[0], mObject.val[4] - mObject.val[1], mObject.val[5] - mObject.val[2]);
            if (dir.isZero()) {
                break;
            }

            double height = dir.getNorm();
            dir.normalize();

            right = (zup ^ dir);
            if (right.isZero()) {
                angle = 0.0;
            } else {
                right.normalize();
                angle = acos(zup | dir);
            }

            glPushMatrix();
            glTranslatef(mObject.val[0], mObject.val[1], mObject.val[2]);
            glRotatef(angle * RAD_TO_DEG, right[0], right[1], right[2]);
            gluCylinder(myQuad, mObject.val[6], mObject.val[7], height,
                        GLuint(mObject.val[8]), GLuint(mObject.val[9]));
            glPopMatrix();
            break;
        }
        case enum_object_torus3d: {
            glEnable(GL_LINE_SMOOTH);
            glPushMatrix();
            glTranslatef(mObject.val[0], mObject.val[1], mObject.val[2]);
            glRotatef(mObject.val[4], 0.0, 0.0, 1.0);
            glRotatef(-mObject.val[3], 1.0, 0.0, 0.0);
            glCallList(mDisplList);
            glPopMatrix();
            glDisable(GL_LINE_SMOOTH);
            break;
        }
        case enum_object_tube3d: {
            glEnable(GL_LINE_SMOOTH);
            glCallList(mDisplList);
            glDisable(GL_LINE_SMOOTH);
            break;
        }
    }
    return true;
}

/*!
 *  \param fptr : file pointer.
 */
void
MyObject::print(FILE* fptr) {
    char* oldlocale = setlocale(LC_NUMERIC, "C");
    fprintf(fptr, "Type      : %s\n", stl_object_type[mObject.type].toStdString().c_str());
    fprintf(fptr, "val  0-2  : %f %f %f\n", mObject.val[0], mObject.val[1], mObject.val[2]);
    fprintf(fptr, "val  3-5  : %f %f %f\n", mObject.val[3], mObject.val[4], mObject.val[5]);
    fprintf(fptr, "val  6-8  : %f %f %f\n", mObject.val[6], mObject.val[7], mObject.val[8]);
    fprintf(fptr, "val  9-11 : %f %f %f\n", mObject.val[9], mObject.val[10], mObject.val[11]);
    fprintf(fptr, "val 12-13 : %f %f\n", mObject.val[12], mObject.val[13]);
    fprintf(fptr, "text      : %s\n", mObject.text.c_str());
    fprintf(fptr, "color     : %f %f %f %f\n", mObject.color[0], mObject.color[1], mObject.color[2], mObject.color[3]);
    fprintf(fptr, "disp-list : %d\n", mDisplList);
    fprintf(fptr, "quadric   : %s\n\n", (myQuad != NULL ? "yes" : "no"));
    setlocale(LC_NUMERIC, oldlocale);
}

/*!
 *  \param line : reference to string.
 */
void
MyObject::printLine(std::string  &line) {
    char* oldlocale = setlocale(LC_NUMERIC, "C");
    char buf[1024];
    switch (mObject.type) {
        case enum_object_undefined:
            break;
        case enum_object_text2d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_text2d].toStdString().c_str(),
                      stl_object_type[enum_object_text2d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.text.c_str(), mObject.val[2],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_text2d].toStdString().c_str(),
                    stl_object_type[enum_object_text2d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.text.c_str(), mObject.val[2],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_text3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_text3d].toStdString().c_str(),
                      stl_object_type[enum_object_text3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.text.c_str(), mObject.val[3],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_text3d].toStdString().c_str(),
                    stl_object_type[enum_object_text3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.text.c_str(), mObject.val[3],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_sphere2d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_sphere2d].toStdString().c_str(),
                      stl_object_type[enum_object_sphere2d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_sphere2d].toStdString().c_str(),
                    stl_object_type[enum_object_sphere2d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_sphere3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_sphere3d].toStdString().c_str(),
                      stl_object_type[enum_object_sphere3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_sphere3d].toStdString().c_str(),
                    stl_object_type[enum_object_sphere3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_box2d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_box2d].toStdString().c_str(),
                      stl_object_type[enum_object_box2d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_box2d].toStdString().c_str(),
                    stl_object_type[enum_object_box2d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_box3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_box3d].toStdString().c_str(),
                      stl_object_type[enum_object_box3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_box3d].toStdString().c_str(),
                    stl_object_type[enum_object_box3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_line2d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_line2d].toStdString().c_str(),
                      stl_object_type[enum_object_line2d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_line2d].toStdString().c_str(),
                    stl_object_type[enum_object_line2d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_line3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_line3d].toStdString().c_str(),
                      stl_object_type[enum_object_line3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_line3d].toStdString().c_str(),
                    stl_object_type[enum_object_line3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_quad2d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_quad2d].toStdString().c_str(),
                      stl_object_type[enum_object_quad2d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_quad2d].toStdString().c_str(),
                    stl_object_type[enum_object_quad2d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_plane3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_plane3d].toStdString().c_str(),
                      stl_object_type[enum_object_plane3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8],
                      mObject.color[0], mObject.color[1], mObject.color[2], mObject.color[3]);
#else
            sprintf(buf, stl_object_format[enum_object_plane3d].toStdString().c_str(),
                    stl_object_type[enum_object_plane3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8],
                    mObject.color[0], mObject.color[1], mObject.color[2], mObject.color[3]);
#endif
            break;
        case enum_object_disk2d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_disk2d].toStdString().c_str(),
                      stl_object_type[enum_object_disk2d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_disk2d].toStdString().c_str(),
                    stl_object_type[enum_object_disk2d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_disk3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_disk3d].toStdString().c_str(),
                      stl_object_type[enum_object_disk3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9],
                      mObject.color[0], mObject.color[1], mObject.color[2], mObject.color[3]);
#else
            sprintf(buf, stl_object_format[enum_object_disk3d].toStdString().c_str(),
                    stl_object_type[enum_object_disk3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9],
                    mObject.color[0], mObject.color[1], mObject.color[2], mObject.color[3]);
#endif
            break;
        case enum_object_cylinder3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_cylinder3d].toStdString().c_str(),
                      stl_object_type[enum_object_cylinder3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_cylinder3d].toStdString().c_str(),
                    stl_object_type[enum_object_cylinder3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_torus3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_torus3d].toStdString().c_str(),
                      stl_object_type[enum_object_torus3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_torus3d].toStdString().c_str(),
                    stl_object_type[enum_object_torus3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
        case enum_object_tube3d:
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_tube3d].toStdString().c_str(),
                      stl_object_type[enum_object_tube3d].toStdString().c_str(),
                      mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9], mObject.val[10], mObject.val[11], mObject.val[12], mObject.val[13],
                      mObject.color[0], mObject.color[1], mObject.color[2]);
#else
            sprintf(buf, stl_object_format[enum_object_tube3d].toStdString().c_str(),
                    stl_object_type[enum_object_tube3d].toStdString().c_str(),
                    mObject.val[0], mObject.val[1], mObject.val[2], mObject.val[3], mObject.val[4], mObject.val[5], mObject.val[6], mObject.val[7], mObject.val[8], mObject.val[9], mObject.val[10], mObject.val[11], mObject.val[12], mObject.val[13],
                    mObject.color[0], mObject.color[1], mObject.color[2]);
#endif
            break;
    }
    std::stringstream ss;
    ss << buf;
    line = ss.str();    
    setlocale(LC_NUMERIC, oldlocale);
}

/*! Print syntax of object.
 *  \param type   : object type.
 *  \param syntax : reference to string.
 */
void
MyObject::printSyntax(enum_object_type type, std::string &syntax) {
    switch (type) {
        case enum_object_undefined: {
            syntax = std::string();
            break;
        }
        case enum_object_text2d: {
            syntax = "text2d  posX posY  text size  col_red col_grenn col_blue";
            break;
        }
        case enum_object_text3d: {
            syntax = "text3d  posX posY posZ  text size  col_red col_grenn col_blue";
            break;
        }
        case enum_object_sphere2d: {
            syntax = "sphere2d  centerX centerY   radius numPoints  line_width  col_red col_green col_blue";
            break;
        }
        case enum_object_sphere3d: {
            syntax = "sphere3d  centerX centerY centerZ   radius slices stacks   col_red col_green col_blue";
            break;
        }
        case enum_object_box2d: {
            syntax = "box2d     centerX centerY   sizeX sizeY  line_width  col_red col_green col_blue";
            break;
        }
        case enum_object_box3d: {
            syntax = "box3d     centerX centerY centerZ   sizeX sizeY sizeZ   line_width  col_red col_green col_blue";
            break;
        }
        case enum_object_line2d: {
            syntax = "line2d     v1_x v1_y   v2_x v2_y   line_width   col_red col_green col_blue";
            break;
        }
        case enum_object_line3d: {
            syntax = "line3d    v1_x v1_y v1_z  v2_x v2_y v2_z   line_width   col_red col_green col_blue";
            break;
        }
        case enum_object_quad2d: {
            syntax = "quad2d    v1_x v1_y  v2_x v2_y  v3_x v3_y  v4_x v4_y  line_width   col_red col_green col_blue";
            break;
        }
        case enum_object_plane3d: {
            syntax = "plane3d   v1_x v1_y v1_z  v2_x v2_y v2_z  v3_x v3_y v3_z  col_red col_green col_blue col_alpha";
            break;
        }
        case enum_object_disk2d: {
            syntax = "disk2d   centerX centerY  radius  slices  col_red col_green col_blue";
            break;
        }
        case enum_object_disk3d: {
            syntax = "disk3d   centerX centerY centerZ  dirX dirY dirZ  iRadius oRadius  slices loops  col_red col_green col_blue col_alpha";
            break;
        }
        case enum_object_cylinder3d: {
            syntax = "cylinder3d  baseX baseY baseZ  topX topY topZ  bRadius tRadius  slices stacks  col_red col_green col_blue";
            break;
        }
        case enum_object_torus3d: {
            syntax = "torus3d   centerX centerY centerZ  normalTheta  normalPhi  outerRadius innerRadius maxPhi  slices stacks  col_red col_green col_blue";
            break;
        }
        case enum_object_tube3d: {
            syntax = "tube3d  baseX baseY baseZ  e1x e1y e1z  e2x e2y e2z  e3x e3y e3z  slices stacks  col_red col_green col_blue";
            break;
        }
    }
}

/*!
 *  \param type : object type.
 *  \param line : reference to string.
 */
void
MyObject::printStandard(enum_object_type type,  std::string &line) {
    char buf[1024];
    switch (type) {
        case enum_object_undefined:
            break;
        case enum_object_text2d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_text2d].toStdString().c_str(),
                      stl_object_type[enum_object_text2d].toStdString().c_str(),
                      0.0, 0.0, "text", 10.0, 0.0, 0.0, 0.0);
#else
            sprintf(buf, stl_object_format[enum_object_text2d].toStdString().c_str(),
                    stl_object_type[enum_object_text2d].toStdString().c_str(),
                    0.0, 0.0, "text", 10.0, 0.0, 0.0, 0.0);
#endif
            break;
        }
        case enum_object_text3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_text3d].toStdString().c_str(),
                      stl_object_type[enum_object_text3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, "text", 10.0, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_text3d].toStdString().c_str(),
                    stl_object_type[enum_object_text3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, "text", 10.0, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_sphere2d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_sphere2d].toStdString().c_str(),
                      stl_object_type[enum_object_sphere2d].toStdString().c_str(),
                      0.0, 0.0, 1.0, 100.0, 1.0, 0.0, 0.0, 0.0);
#else
            sprintf(buf, stl_object_format[enum_object_sphere2d].toStdString().c_str(),
                    stl_object_type[enum_object_sphere2d].toStdString().c_str(),
                    0.0, 0.0, 1.0, 100.0, 1.0, 0.0, 0.0, 0.0);
#endif
            break;
        }
        case enum_object_sphere3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_sphere3d].toStdString().c_str(),
                      stl_object_type[enum_object_sphere3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, 1.0, DEF_GLU_SPHERE_SLICES, DEF_GLU_SPHERE_STACKS, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_sphere3d].toStdString().c_str(),
                    stl_object_type[enum_object_sphere3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, 1.0, DEF_GLU_SPHERE_SLICES, DEF_GLU_SPHERE_STACKS, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_box2d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_box2d].toStdString().c_str(),
                      stl_object_type[enum_object_box2d].toStdString().c_str(),
                      0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0);
#else
            sprintf(buf, stl_object_format[enum_object_box2d].toStdString().c_str(),
                    stl_object_type[enum_object_box2d].toStdString().c_str(),
                    0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0);
#endif
            break;
        }
        case enum_object_box3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_box3d].toStdString().c_str(),
                      stl_object_type[enum_object_box3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_box3d].toStdString().c_str(),
                    stl_object_type[enum_object_box3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_line2d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_line2d].toStdString().c_str(),
                      stl_object_type[enum_object_line2d].toStdString().c_str(),
                      0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0);
#else
            sprintf(buf, stl_object_format[enum_object_line2d].toStdString().c_str(),
                    stl_object_type[enum_object_line2d].toStdString().c_str(),
                    0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0);
#endif
            break;
        }
        case enum_object_line3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_line3d].toStdString().c_str(),
                      stl_object_type[enum_object_line3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_line3d].toStdString().c_str(),
                    stl_object_type[enum_object_line3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_quad2d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_quad2d].toStdString().c_str(),
                      stl_object_type[enum_object_quad2d].toStdString().c_str(),
                      -1.0, -1.0,  1.0, -1.0,  1.0, 1.0,  -1.0, 1.0,  1.0,  0.0, 0.0, 0.0);
#else
            sprintf(buf, stl_object_format[enum_object_quad2d].toStdString().c_str(),
                    stl_object_type[enum_object_quad2d].toStdString().c_str(),
                    -1.0, -1.0,  1.0, -1.0,  1.0, 1.0,  -1.0, 1.0,  1.0,  0.0, 0.0, 0.0);
#endif
            break;
        }
        case enum_object_plane3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_plane3d].toStdString().c_str(),
                      stl_object_type[enum_object_plane3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_plane3d].toStdString().c_str(),
                    stl_object_type[enum_object_plane3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_disk2d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_disk2d].toStdString().c_str(),
                      stl_object_type[enum_object_disk2d].toStdString().c_str(),
                      0.0, 0.0, 1.0, 40.0, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_disk2d].toStdString().c_str(),
                    stl_object_type[enum_object_disk2d].toStdString().c_str(),
                    0.0, 0.0, 1.0, 40.0, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_disk3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_disk3d].toStdString().c_str(),
                      stl_object_type[enum_object_disk3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, DEF_GLU_DISK_SLICES, DEF_GLU_DISK_LOOPS, 1.0, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_disk3d].toStdString().c_str(),
                    stl_object_type[enum_object_disk3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, DEF_GLU_DISK_SLICES, DEF_GLU_DISK_LOOPS, 1.0, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_cylinder3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_cylinder3d].toStdString().c_str(),
                      stl_object_type[enum_object_cylinder3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, DEF_GLU_CYLINDER_SLICES, DEF_GLU_CYLINDER_STACKS, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_cylinder3d].toStdString().c_str(),
                    stl_object_type[enum_object_cylinder3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, DEF_GLU_CYLINDER_SLICES, DEF_GLU_CYLINDER_STACKS, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_torus3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_torus3d].toStdString().c_str(),
                      stl_object_type[enum_object_torus3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.5, 360.0, 40.0, 20.0, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_torus3d].toStdString().c_str(),
                    stl_object_type[enum_object_torus3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.5, 360.0, 40.0, 20.0, 1.0, 1.0, 1.0);
#endif
            break;
        }
        case enum_object_tube3d: {
#ifdef _WIN32
            sprintf_s(buf, stl_object_format[enum_object_tube3d].toStdString().c_str(),
                      stl_object_type[enum_object_tube3d].toStdString().c_str(),
                      0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, DEF_GLU_CYLINDER_SLICES, DEF_GLU_CYLINDER_STACKS, 1.0, 1.0, 1.0);
#else
            sprintf(buf, stl_object_format[enum_object_tube3d].toStdString().c_str(),
                    stl_object_type[enum_object_tube3d].toStdString().c_str(),
                    0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, DEF_GLU_CYLINDER_SLICES, DEF_GLU_CYLINDER_STACKS, 1.0, 1.0, 1.0);
#endif
            break;
        }
    }
    line = std::string(buf);
}


/*! Create quadric for GLU objects.
 */
void
MyObject::createQuadric() {
    deleteQuadric();

    myQuad = gluNewQuadric();
    gluQuadricOrientation(myQuad, GLU_OUTSIDE);
    gluQuadricDrawStyle(myQuad, mObject.style);
    gluQuadricTexture(myQuad, GL_FALSE);
    gluQuadricNormals(myQuad, GLU_SMOOTH);
}

/*! Delete quadric.
 */
void
MyObject::deleteQuadric() {
    if (myQuad != NULL) {
        gluDeleteQuadric(myQuad);
    }
    myQuad = NULL;
}

/*! Clamp value.
 *  \param x : value to be clamped.
 *  \param min : minimum value.
 *  \param max : maximum value.
 */
float
MyObject::clamp_float(float x, const float min, const float max) {
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    }
    return x;
}
