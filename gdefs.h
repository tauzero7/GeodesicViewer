/**
 * @file    gdefs.h
 * @author  Thomas Mueller
 *
 * @brief  Global definitions for the GeodesicViewer.
 *
 * This file is part of GeodesicView.
 */
#ifndef GDEFS_H
#define GDEFS_H

#include <QColor>
#include <QStringList>
#include <m4dGlobalDefs.h>
#include <map>

// must be before gl.h includes
#ifdef _WIN32
#include <windows.h>
#endif

#include <QOpenGLFunctions>

// -----------------------------------
//   GUI definitions
// -----------------------------------
#define DEF_GUI_WIDTH 1000
#define DEF_GUI_HEIGHT 500
#define DEF_DRAW2D_WIDTH 720
#define DEF_OPENGL_WIDTH 720
#define DEF_OPENGL_HEIGHT 590

#define DEF_INIT_EYE_POS_X 0.0
#define DEF_INIT_EYE_POS_Y 0.0
#define DEF_INIT_EYE_POS_Z 25.0
#define DEF_INIT_POI_X 0.0
#define DEF_INIT_POI_Y 0.0
#define DEF_INIT_POI_Z 0.0
#define DEF_INIT_VUP_X 0.0
#define DEF_INIT_VUP_Y 1.0
#define DEF_INIT_VUP_Z 0.0
#define DEF_INIT_FOV 50.0

#define DEF_STEREO_SEP 0.1
#define DEF_STEREO_STEP 0.01
#define DEF_STEREO_TYPE 0

#define DEF_PREC_COMPASS 12
#define DEF_PREC_EMBEDDING 12
#define DEF_PREC_SCALE 6
#define DEF_PREC_DRAW 12
#define DEF_PREC_STEREO 6
#define DEF_PREC_ANIM 6
#define DEF_PREC_POSITION 14
#define DEF_PREC_VELOCITY 10
#define DEF_PREC_METRIC_PAR 12
#define DEF_PREC_BOOST 6
#define DEF_PREC_JACOBI 6

#define DEF_ZOOM_STEP 0.5

#define DEF_2D_FILE_FILTER "*.2d.png"
#define DEF_2D_FILE_ENDING ".2d.png"
#define DEF_3D_FILE_FILTER "*.3d.png"
#define DEF_3D_FILE_ENDING ".3d.png"
#define DEF_IMG_FILE_FILTER "*.png"
#define DEF_IMG_FILE_ENDING ".png"

// -----------------------------------
//   mouse handling
// -----------------------------------
enum enum_mouse_handle {
    enum_mouse_rotate = 0,
    enum_mouse_rotate_global,
    enum_mouse_rotate_sphere,
    enum_mouse_camera_dist,
    enum_mouse_move_local,
    enum_mouse_move_global,
    enum_mouse_move_poi
};

const QStringList stl_mouse_handle = QStringList() << "rotate local"
                                                   << "rotate around xyz"
                                                   << "rotate on sphere"
                                                   << "camera distance"
                                                   << "move along camera"
                                                   << "move along xyz"
                                                   << "move poi";

// -----------------------------------
//   camera rotation
// -----------------------------------
#define DEF_CAM_ROT_LOCAL_VUP Qt::LeftButton
#define DEF_CAM_ROT_LOCAL_VUP_XY dxy.x()
#define DEF_CAM_ROT_LOCAL_VUP_SCALE 0.003
#define DEF_CAM_ROT_LOCAL_RIGHT Qt::MidButton
#define DEF_CAM_ROT_LOCAL_RIGHT_XY dxy.y()
#define DEF_CAM_ROT_LOCAL_RIGHT_SCALE 0.003
#define DEF_CAM_ROT_LOCAL_DIR Qt::RightButton
#define DEF_CAM_ROT_LOCAL_DIR_XY dxy.x()
#define DEF_CAM_ROT_LOCAL_DIR_SCALE 0.003

#define DEF_CAM_ROT_GLOBAL_X Qt::RightButton
#define DEF_CAM_ROT_GLOBAL_X_XY dxy.x()
#define DEF_CAM_ROT_GLOBAL_X_SCALE -0.003
#define DEF_CAM_ROT_GLOBAL_Y Qt::MidButton
#define DEF_CAM_ROT_GLOBAL_Y_XY dxy.y()
#define DEF_CAM_ROT_GLOBAL_Y_SCALE 0.003
#define DEF_CAM_ROT_GLOBAL_Z Qt::LeftButton
#define DEF_CAM_ROT_GLOBAL_Z_XY dxy.x()
#define DEF_CAM_ROT_GLOBAL_Z_SCALE 0.003

#define DEF_CAM_ROT_SPHERE_THETA_PHI Qt::LeftButton
#define DEF_CAM_ROT_SPHERE_TP_SCALE 0.01
#define DEF_CAM_ROT_SPHERE_DISTANCE Qt::MidButton
#define DEF_CAM_ROT_SPHERE_DIST_SCALE 0.05

// -----------------------------------
//   camera draw style
// -----------------------------------
enum enum_draw_style { enum_draw_points = 0, enum_draw_lines };

const QStringList stl_draw_styles = QStringList() << "points"
                                                  << "lines";

// -----------------------------------
//   camera projection
// -----------------------------------
enum enum_projection { enum_proj_perspective = 0, enum_proj_orthographic };

const QStringList stl_projection = QStringList() << "perspective"
                                                 << "orthographics";

// -----------------------------------
//   camera predefined orientation
// -----------------------------------
enum enum_camera_predefs { enum_camera_xy = 0, enum_camera_zx, enum_camera_yz, enum_camera_custom };

const QStringList stl_camera_predefs = QStringList() << "xy"
                                                     << "zx"
                                                     << "yz"
                                                     << "custom";

// -----------------------------------
//   stereo glasses
// -----------------------------------
enum enum_stereo_glasses {
    enum_stereo_red_blue = 0,
    enum_stereo_red_green,
    enum_stereo_red_cyan,
    enum_stereo_blue_red,
    enum_stereo_green_red,
    enum_stereo_cyan_red
};

const QStringList stl_stereo_glasses = QStringList() << "red - blue"
                                                     << "red - green"
                                                     << "red - cyan"
                                                     << "blue - red"
                                                     << "green - red"
                                                     << "cyan - red";

enum enum_stereo_type { enum_stereo_off_axis = 0, enum_stereo_parallel, enum_stereo_toe_in };

const QStringList stl_stereo_types = QStringList() << "off_axis"
                                                   << "parallel"
                                                   << "toe_in";

// -----------------------------------
//   initial direction orientation
// -----------------------------------
enum enum_initdir_orient { enum_initdir_axis_3 = 0, enum_initdir_axis_1, enum_initdir_axis_2 };

const QStringList stl_initdir_orient = QStringList() << "axis-3"
                                                     << "axis-1"
                                                     << "axis-2";

// -----------------------------------
//   Sachs legs, system
// -----------------------------------
enum enum_sachs_legs {
    enum_sachs_legs_right_up = 0,
    enum_sachs_legs_up_left,
    enum_sachs_legs_left_down,
    enum_sachs_legs_down_right,
    enum_sachs_legs_all
};

const QStringList stl_sachs_legs = QStringList() << "right-up"
                                                 << "up-left"
                                                 << "left-down"
                                                 << "down-right"
                                                 << "all";

enum enum_sachs_system { enum_sachs_e1e2e3 = 0, enum_sachs_e2e3e1, enum_sachs_e3e1e2 };

const QStringList stl_sachs_system = QStringList() << "e1-e2-e3"
                                                   << "e2-e3-e1"
                                                   << "e3-e1-e2";

enum enum_jacobi_param {
    enum_jacobi_dp = 0,
    enum_jacobi_dm,
    enum_jacobi_mu,
    enum_jacobi_angle,
    enum_jacobi_ellipt,
    enum_jacobi_ellipse
};

enum enum_draw_coord_num {
    enum_draw_coord_x0 = 0,
    enum_draw_coord_x1,
    enum_draw_coord_x2,
    enum_draw_coord_x3,
    enum_draw_lambda,
    enum_draw_coord_dx0,
    enum_draw_coord_dx1,
    enum_draw_coord_dx2,
    enum_draw_coord_dx3
};

// -----------------------------------
//   valuestep_model
// -----------------------------------
#define DEF_VALUE_STEP 0.01
#define DEF_NUM_DECIMALS 6

#define DEF_INIT_CHI 90.0
#define DEF_INIT_CHI_STEP 0.1
#define DEF_INIT_KSI 0.0
#define DEF_INIT_KSI_STEP 0.1
#define DEF_INIT_VEL 0.0
#define DEF_INIT_VEL_STEP 0.01

#define DEF_MAX_NUM_POINTS 3000

#ifndef SIGNUM
#define SIGNUM(x) (x >= 0 ? 1.0 : -1.0)
#endif

// -----------------------------------
//   GUI definitions
// -----------------------------------
#define DEF_STANDARD_DATA_DIRECTORY "data"

#define DEF_PROTOCOL_FILE_ENDING ".ini"
#define DEF_PROTOCOL_FILE_ENDING_PATTERN "*.ini"

#define DEF_VPARAMS_FILE_ENDING ".cfg"
#define DEF_VPARAMS_FILE_ENDING_PATTERN "*.cfg"

#define DEF_OBJECT_FILE_ENDING ".obj"
#define DEF_OBJECT_FILE_ENDING_PATTERN "*.obj"

#define DEF_REPORT_FILE_ENDING ".rep"
#define DEF_REPORT_FILE_ENDING_PATTERN "*.rep"

#define DEF_DRAW2D_X_INIT_MIN -10.0
#define DEF_DRAW2D_X_INIT_MAX 10.0
#define DEF_DRAW2D_Y_INIT_MIN -10.0
#define DEF_DRAW2D_Y_INIT_MAX 10.0

#define DEF_DRAW3D_SCALE_X_STEP 0.1
#define DEF_DRAW3D_SCALE_Y_STEP 0.1
#define DEF_DRAW3D_SCALE_Z_STEP 0.1

#define DEF_OPENGL_FOV_STEP 1.0
#define DEF_OPENGL_EYE_STEP 0.1
#define DEF_OPENGL_POI_STEP 0.1

#define DEF_OPENGL_FOG_STEP 0.01
#define DEF_OPENGL_FOG_DENSITY_INIT 0.05

#define DEF_OPENGL_LINE_COLOR 255, 255, 0
#define DEF_OPENGL_LINE_WIDTH 1 // range: 1-5
#define DEF_OPENGL_BG_COLOR 0, 0, 0

#define DEF_OPENGL_LEG1_COL1 255, 255, 0
#define DEF_OPENGL_LEG1_COL2 0, 255, 255
#define DEF_OPENGL_LEG2_COL1 255, 0, 255
#define DEF_OPENGL_LEG2_COL2 0, 255, 255
#define DEF_OPENGL_LEG1_FREQ 40.0
#define DEF_OPENGL_LEG2_FREQ 40.0

#define DEF_OPENGL_EMB_COLOR 192, 192, 192

#define DEF_OPENGL_ANIM_ROT_X_INIT 0.0
#define DEF_OPENGL_ANIM_ROT_Y_INIT 0.0
#define DEF_OPENGL_ANIM_ROT_Z_INIT 0.0

#define DEF_DOUBLE_EDIT_COLOR 200, 240, 255

#define DEF_DRAW2D_BG_COLOR 220, 220, 220
#define DEF_DRAW2D_LINE_COLOR 0, 0, 255
#define DEF_DRAW2D_LINE_WIDTH 1
#define DEF_DRAW2D_GRID_COLOR 0, 0, 0

#define DEF_DRAW2D_LEFT_BORDER 30
#define DEF_DRAW2D_BOTTOM_BORDER 30

#define DEF_DRAW2D_ZOOM_COLOR 35, 35, 35

// Parameter configuration
typedef struct _struct_params {
    std::string filename;

    // initial angles, velocity, ...
    int axis_orientation;
    double geod_chi_init;
    double geod_chi_step;
    double geod_ksi_init;
    double geod_ksi_step;
    double geod_vel_init;
    double geod_vel_step;
    int geod_max_num_points_init;

    int opengl_projection;
    m4d::enum_draw_type opengl_draw3d_type;

    std::map<std::string, double> opengl_emb_params;
    QColor opengl_emb_color;

    m4d::vec3 opengl_eye_pos;
    double opengl_eye_pos_step;
    m4d::vec3 opengl_eye_poi;
    double opengl_eye_poi_step;
    m4d::vec3 opengl_eye_vup;
    m4d::vec3 opengl_eye_dir;
    double opengl_fov;
    double opengl_fov_step;

    QColor opengl_line_color;
    QColor opengl_bg_color;
    int opengl_line_width;
    int opengl_line_smooth;

    int opengl_stereo_use;
    enum_stereo_glasses opengl_stereo_glasses;
    enum_stereo_type opengl_stereo_type;
    double opengl_stereo_sep;
    double opengl_stereo_step;

    int opengl_fog_use;
    double opengl_fog_init;
    double opengl_fog_step;

    double opengl_scale_x;
    double opengl_scale_y;
    double opengl_scale_z;

    int opengl_anim_local;
    double opengl_anim_rot_x;
    double opengl_anim_rot_y;
    double opengl_anim_rot_z;

    QColor opengl_leg1_col1;
    QColor opengl_leg1_col2;
    double opengl_leg1_freq;
    QColor opengl_leg2_col1;
    QColor opengl_leg2_col2;
    double opengl_leg2_freq;

    enum_sachs_system opengl_sachs_system;
    enum_sachs_legs opengl_sachs_legs;
    double opengl_sachs_scale;

    double zoom_step;

    double draw2d_xMin;
    double draw2d_xMax;
    double draw2d_yMin;
    double draw2d_yMax;
    QColor draw2d_bg_color;
    QColor draw2d_line_color;
    int draw2d_line_width;
    int draw2d_line_smooth;
    QColor draw2d_grid_color;

    m4d::enum_draw_type draw2d_representation;
    int draw2d_abscissa;
    int draw2d_ordinate;

} struct_params;

// -----------------------------------
//   objects
// -----------------------------------
#define DEF_GLU_SPHERE_SLICES 40.0
#define DEF_GLU_SPHERE_STACKS 40.0

#define DEF_GLU_DISK_SLICES 40.0
#define DEF_GLU_DISK_LOOPS 10.0

#define DEF_GLU_CYLINDER_SLICES 20.0
#define DEF_GLU_CYLINDER_STACKS 20.0

enum enum_object_type {
    enum_object_undefined = 0,
    enum_object_sphere2d,
    enum_object_sphere3d,
    enum_object_box2d,
    enum_object_box3d,
    enum_object_line2d,
    enum_object_line3d,
    enum_object_quad2d,
    enum_object_plane3d,
    enum_object_disk2d,
    enum_object_disk3d,
    enum_object_cylinder3d,
    enum_object_torus3d,
    enum_object_tube3d,
    enum_object_text2d,
    enum_object_text3d
};

const QStringList stl_object_type = QStringList() << "undefined"
                                                  << "sphere2d"
                                                  << "sphere3d"
                                                  << "box2d"
                                                  << "box3d"
                                                  << "line2d"
                                                  << "line3d"
                                                  << "quad2d"
                                                  << "plane3d"
                                                  << "disk2d"
                                                  << "disk3d"
                                                  << "cylinder3d"
                                                  << "torus3d"
                                                  << "tube3d"
                                                  << "text2d"
                                                  << "text3d";

#define DEF_STRING_FORMAT QString("%-10s")
#define DEF_TEXT_FORMAT QString("%-10s")
#define DEF_FLOAT_FORMAT QString("%8.4f")
#define DEF_SHORT_FORMAT QString("%4.1f")
#define DEF_COLOR_FORMAT QString("%5.2f")

const QStringList stl_object_format = QStringList()
    << QString() // enum_object_undefined
    << QString(DEF_STRING_FORMAT + " " // enum_object_sphere2d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_sphere3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " "
           + DEF_SHORT_FORMAT + " " + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " "
           + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_box2d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_box3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " "
           + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_line2d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_line3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " "
           + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_quad2d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_plane3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " "
           + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_disk2d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + "  " + DEF_SHORT_FORMAT + "  "
           + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_disk3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_SHORT_FORMAT + " " + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " "
           + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_cylinder3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_SHORT_FORMAT + " " + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " "
           + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_torus3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_SHORT_FORMAT + " " + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " "
           + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_tube3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " "
           + DEF_FLOAT_FORMAT + "  " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_SHORT_FORMAT + " " + DEF_SHORT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " "
           + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_text2d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_TEXT_FORMAT + " " + DEF_FLOAT_FORMAT + "  "
           + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT)
    << QString(DEF_STRING_FORMAT + " " // enum_object_text3d
           + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + " " + DEF_FLOAT_FORMAT + "  " + DEF_TEXT_FORMAT + " "
           + DEF_FLOAT_FORMAT + "  " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT + " " + DEF_COLOR_FORMAT);

enum enum_object_dim { enum_object_dim_0d = 0, enum_object_dim_1d, enum_object_dim_2d, enum_object_dim_3d };

const QStringList stl_object_dim = QStringList() << "0d"
                                                 << "1d"
                                                 << "2d"
                                                 << "3d";

const int NUM_OBJECT_MAX_VALUES = 14;

typedef struct _struct_obj {
    enum_object_type type;
    enum_object_dim dim;

    float val[NUM_OBJECT_MAX_VALUES];
    int numvals;
    float color[4];

    GLuint style;
    bool lighted;

    std::string text;
} struct_obj;

#define DEF_DRAW2D_X_STEP 12
#define DEF_DRAW2D_Y_STEP 12
const QStringList dbl_draw2d_steps = QStringList() << "1e-8"
                                                   << "1e-7"
                                                   << "1e-6"
                                                   << "1e-5"
                                                   << "1e-4"
                                                   << "1e-3"
                                                   << "0.01"
                                                   << "0.02"
                                                   << "0.05"
                                                   << "0.1"
                                                   << "0.2"
                                                   << "0.5"
                                                   << "1.0"
                                                   << "2.0"
                                                   << "5.0"
                                                   << "10.0"
                                                   << "20.0"
                                                   << "50.0"
                                                   << "100.0"
                                                   << "200.0"
                                                   << "500.0"
                                                   << "1000.0"
                                                   << "10000.0"
                                                   << "100000.0"
                                                   << "1000000.0";

#define DEF_SOCKET_PORT 9900

enum enum_socket_task { enum_stask_initdir = 0, enum_stask_initpos };

typedef struct gvs_socket_data_t {
    int task;
    double vals[4];
} gvs_socket_data;

#endif
