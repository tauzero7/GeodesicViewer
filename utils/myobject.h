/**
 * @file    myobject.h
 * @author  Thomas Mueller
 *
 * @brief  Define an object in 2d/3d.

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
        disk3d  centerX centerY centerZ  dirX   dirY   dirZ    iRadius oRadius  slices  loops   col_red  col_green col_blue<br>
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
 *
 * This file is part of GeodesicView.
 */
#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <iostream>
#include <sstream>

//must be before gl.h includes
#ifdef _WIN32
#include <windows.h>
#endif

#include <gdefs.h>
#include <m4dGlobalDefs.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

const GLfloat color_white[4] = { 1.0, 1.0, 1.0, 1.0 };

/**
 * @brief The MyObject class
 */
class MyObject {
public:
    MyObject(bool prepare = true);
    MyObject(float red, float green, float blue, float alpha = 1.0);
    MyObject(const MyObject& obj);
    virtual ~MyObject();

public:
    enum_object_type getObjectType();
    enum_object_dim getObjectDim();
    void getObject(struct_obj& obj);

    bool getValue(int num, double& val);
    std::string getText();

    void setColor(float red, float green, float blue, float alpha = 1.0);
    void getColor(float& red, float& green, float& blue);
    void getColor(float& red, float& green, float& blue, float& alpha);
    bool withLight(float mat[], bool stereo = false);

    void create_object(enum_object_type type, double *val, std::string text = "");

    void create_2d_text(double cx, double cy, std::string text, double size);
    void create_3d_text(double cx, double cy, double cz, std::string text, double size);

    void create_2d_sphere(double cx, double cy, double radius, double num_points, double width);
    void create_3d_sphere(double cx, double cy, double cz,
        double radius, double slices = DEF_GLU_SPHERE_SLICES, double stacks = DEF_GLU_SPHERE_STACKS);

    void create_2d_box(double cx, double cy, double sizeX, double sizeY, double width);
    void create_3d_box(double cx, double cy, double cz, double sizeX, double sizeY, double sizeZ, double width);

    void create_2d_line(double lx, double ly, double ux, double uy, double width);
    void create_3d_line(double lx, double ly, double lz, double ux, double uy, double uz, double width);

    void create_2d_quad(double v1x, double v1y, double v2x, double v2y, double v3x, double v3y, double v4x, double v4y, double width);

    void create_3d_plane(double v1x, double v1y, double v1z,
        double v2x, double v2y, double v2z,
        double v3x, double v3y, double v3z);

    void create_2d_disk(double cx, double cy, double radius, double fans = 30);

    void create_3d_disk(double cx, double cy, double cz,
        double dx, double dy, double dz,
        double  iRadius, double  oRadius,
        double  slices = DEF_GLU_DISK_SLICES, double  loops = DEF_GLU_DISK_LOOPS);

    void create_3d_cylinder(double  bx, double  by, double  bz, double  tx, double  ty, double  tz,
        double  baseRadius, double  topRadius,
        double  slices = DEF_GLU_SPHERE_SLICES, double  stacks = DEF_GLU_SPHERE_STACKS);

    void create_3d_torus(double  cx, double  cy, double  cz, double  nt, double  np,
        double  outerRadius, double  innerRadius, double  maxPhi,
        double  slices = DEF_GLU_SPHERE_SLICES, double  stacks = DEF_GLU_SPHERE_STACKS);

    void create_3d_tube(double  bx, double  by, double  bz, double  e1x, double  e1y, double  e1z,
        double  e2x, double  e2y, double  e2z, double  e3x, double  e3y, double  e3z,
        double  slices = DEF_GLU_SPHERE_SLICES, double  stacks = DEF_GLU_SPHERE_STACKS);

    bool drawObject(bool stereo = false);

    void print(FILE* fptr = stderr);
    void printLine(std::string& line);

    static void printSyntax(enum_object_type type, std::string& syntax);
    static void printStandard(enum_object_type type, std::string& line);

protected:
    void createQuadric();
    void deleteQuadric();

    void torus(const double  R, const double  r, const double  alpha, const double  beta,
        double & x, double & y, double & z, double & dx, double & dy, double & dz);

protected:
    struct_obj mObject;
    GLUquadric* myQuad;
    GLuint mDisplList;
    bool mPrepare;
};

#endif
