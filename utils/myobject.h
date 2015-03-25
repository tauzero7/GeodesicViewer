// --------------------------------------------------------------------------------
/*
    object.h

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

/*!  \class  MyObject
     \brief  Define an object in 2d/3d.

     To create a 2d-text:<br>
      text2d    posX   posY    text size    col_red  col_green col_blue<br>
           ->   val[0] val[1]  text val[3]  color[0] color[1]  color[2]

     To create a 3d-text:<br>
      text3d    posX   posY   posY   text size    col_red  col_green col_blue<br>
           ->   val[0] val[1] val[2] text val[3]  color[0] color[1]  color[2]

     To create a 2d-sphere:<br>
      sphere2d  centerX centerY  radius   numPoints   col_red  col_green col_blue<br>
           ->   val[0]  val[1]   val[2]   val[3]      color[0] color[1]  color[2]

     To create a 3d-sphere:<br>
      sphere3d  centerX centerY centerZ   radius   slices  stacks   col_red  col_green col_blue<br>
           ->   val[0]  val[1]  val[2]    val[3]   val[4]  val[5]  color[0] color[1]  color[2]

     To create a 2d-box:<br>
      box2d  centerX centerY  sizeX   sizeY    col_red  col_green col_blue<br>
         ->  val[0]  val[1]   val[2]  val[3]   color[0] color[1]  color[2]

     To create a 3d-box:<br>
      box3d  centerX centerY centerZ  sizeX   sizeY   sizeZ   line_width   col_red  col_green col_blue<br>
         ->  val[0]  val[1]  val[2]   val[3]  val[4]  val[5]  val[6]       color[0] color[1]  color[2]

     To create a 2d-line:<br>
      line2d  vert1_X vert1_Y  vert2_X vert2_Y   line_width   col_red  col_green col_blue<br>
         ->   val[0]  val[1]   val[2]  val[3]    val[4]       color[0] color[1]  color[2]

     To create a 3d-line:<br>
      line3d  vert1_X vert1_Y vert1_Z  vert2_X vert2_Y vert2_Z  line_width  col_red  col_green col_blue<br>
         ->   val[0]  val[1]  val[2]   val[3]  val[4]  val[5]   val[6]      color[0] color[1]  color[2]

     To create a 2d-quad:<br>
      quad2d  vert1_X vert1_Y  vert2_X vert2_Y  vert3_X vert3_Y  vert4_X vert4_Y  line_width  col_red  col_green col_blue<br>
         ->   val[0]  val[1]   val[2]  val[3]   val[4]  val[5]   val[6]  val[7]   val[8]      color[0] color[1]  color[2]

     To create a 3d-plane:<br>
      plane3d  vert1_x vect1_y vert1_z  vert2_x vert2_y vert2_z  vert3_x vert3_y vert3 z  col_red  col_green col_blue<br>
         ->    val[0]  val[1]  val[2]   val[3]  val[4]  val[5]   val[6]  val[7]  val[8]   color[0] color[1]  color[2]

     To create a 3d-disk:<br>
      distk3d  centerX centerY centerZ  dirX   dirY   dirZ    iRadius oRadius  slices  loops   col_red  col_green col_blue<br>
         ->    val[0]  val[1]  val[2]   val[3] val[4] val[5]  val[6]  val[7]   val[8]  val[9]  color[0] color[1]  color[2]

     To create a 3d-cylinder:<br>
      cylinder3d  baseX  baseY  baseZ   topX   topY   topZ   bRadius tRadius  slices stacks  col_red  col_green col_blue<br>
         ->       val[0] val[1] val[2]  val[3] val[4] val[5] val[6]  val[7]   val[8] val[9]  color[0] color[1]  color[2]

     To create a 3d-torus:<br>
      torus3d   centerX  centerY  centerZ  normalTheta  normalPhi  outerRadius  innerRadius  maxPhi  slices  stacks  col_red  col_green col_blue<br>
         ->     val[0]   val[1]   val[2]   val[3]       val[4]     val[5]       val[6]       val[7]  val[8]  val[9]  color[0] color[1]  color[2]

     To create a 3d-tube:<br>
      tube3d    baseX  baseY  baseZ   e1x    e1y    e1z     e2x    e2y    e2z     e3x    e3y     e3z      col_red  col_green col_blue<br>
         ->     val[0] val[1] val[2]  val[3] val[4] val[5]  val[6] val[7] val[8]  val[9] val[10] val[11]  color[0] color[1]  color[2]
*/
// --------------------------------------------------------------------------------

#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <iostream>
#include <sstream>

//must be before gl.h includes
#ifdef _WIN32
#include <windows.h>
#endif

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <gl.h>
#include <glu.h>
#endif

#include <gdefs.h>
#include <m4dGlobalDefs.h>
#include <GL/glu.h>

const GLfloat color_white[4] = {1.0,1.0,1.0,1.0};

// ---------------------------------------------------
//     class definition:   object
// ---------------------------------------------------
class MyObject {
public:
    MyObject(bool prepare = true);
    MyObject(float red, float green, float blue, float alpha = 1.0);
    MyObject(const MyObject &obj);
    virtual ~MyObject();

public:
    enum_object_type  getObjectType();
    enum_object_dim   getObjectDim();
    void              getObject(struct_obj &obj);

    bool              getValue(int num, float &val);
    std::string       getText();

    void  setColor(float red, float green, float blue, float alpha = 1.0);
    void  getColor(float &red, float &green, float &blue);
    void  getColor(float &red, float &green, float &blue, float &alpha);
    bool  withLight(float light[], bool stereo = false);

    void  create_object(enum_object_type  type, float* val, std::string text = "");

    void  create_2d_text(float cx, float cy, std::string text, float size);
    void  create_3d_text(float cx, float cy, float cz, std::string text, float size);

    void  create_2d_sphere(float cx, float cy, float radius, float num_points, float width);
    void  create_3d_sphere(float cx, float cy, float cz,
                           float radius, float slices = DEF_GLU_SPHERE_SLICES, float stacks = DEF_GLU_SPHERE_STACKS);

    void  create_2d_box(float cx, float cy, float sizeX, float sizeY, float width);
    void  create_3d_box(float cx, float cy, float cz, float sizeX, float sizeY, float sizeZ, float width);

    void  create_2d_line(float lx, float ly, float ux, float uy, float width);
    void  create_3d_line(float lx, float ly, float lz, float ux, float uy, float uz, float width);

    void  create_2d_quad(float v1x, float v1y, float v2x, float v2y, float v3x, float v3y, float v4x, float v4y, float width);

    void  create_3d_plane(float v1x, float v1y, float v1z,
                          float v2x, float v2y, float v2z,
                          float v3x, float v3y, float v3z);

    void  create_2d_disk(float cx, float cy, float radius, float fans = 30);

    void  create_3d_disk(float cx, float cy, float cz,
                         float dx, float dy, float dz,
                         float iRadius, float oRadius,
                         float slices = DEF_GLU_DISK_SLICES, float loops = DEF_GLU_DISK_LOOPS);

    void  create_3d_cylinder(float bx, float by, float bz, float tx, float ty, float tz,
                             float baseRadius, float topRadius,
                             float slices = DEF_GLU_SPHERE_SLICES, float stacks = DEF_GLU_SPHERE_STACKS);

    void  create_3d_torus(float cx, float cy, float cz, float nt, float np,
                          float outerRadius, float innerRadius, float maxPhi,
                          float slices = DEF_GLU_SPHERE_SLICES, float stacks = DEF_GLU_SPHERE_STACKS);

    void  create_3d_tube(float bx, float by, float bz,  float e1x, float e1y, float e1z,
                         float e2x, float e2y, float e2z,  float e3x, float e3y, float e3z,
                         float slices = DEF_GLU_SPHERE_SLICES, float stacks = DEF_GLU_SPHERE_STACKS);

    bool  drawObject(bool stereo = false);

    void  print(FILE* fptr = stderr);
    void  printLine(std::string  &line);

    static void printSyntax(enum_object_type type, std::string &syntax);
    static void printStandard(enum_object_type type, std::string &line);

protected:
    void  createQuadric();
    void  deleteQuadric();
    float clamp_float(float x, const float min, const float max);

    void  torus(const float R, const float r, const float alpha, const float beta,
                float &x, float &y, float &z, float &dx, float &dy, float &dz);

protected:
    struct_obj   mObject;
    GLUquadric*  myQuad;
    GLuint       mDisplList;
    bool         mPrepare;
};

#endif
