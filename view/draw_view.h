/**
 * @file    draw_view.h
 * @author  Thomas Mueller
 *
 * @brief  View for draw manipulation.
 *
     <ul>
       <li>General 3D parameters<br><img src="../pics/draw_3d.png">
       <li>3D scaling<br><img src="../pics/draw_3d_scale.png">
       <li>3D stereo<br><img src="../pics/draw_3d_stereo.png">
       <li>3D animations<br><img src="../pics/draw_3d_animate.png">
       <li>3D embeddings<br><img src="../pics/draw_3d_embed.png">
       <li>General 2D parameters<br><img src="../pics/draw_2d.png">
     </ul>

 * This file is part of GeodesicView.
 */
#ifndef DRAW_VIEW_H
#define DRAW_VIEW_H

#include <iostream>

#include <QCheckBox>
#include <QComboBox>
#include <QLCDNumber>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableWidget>

#include <doubleedit_util.h>
#include <gdefs.h>
#include <greek.h>

#include <opengl2d_model.h>
#include <opengl3d_model.h>
#include <openglJacobi_model.h>

#include <extra/m4dObject.h>
#include <m4dGlobalDefs.h>

/**
 * @brief The DrawView class
 */
class DrawView : public QWidget {
    Q_OBJECT

public:
    DrawView(OpenGL3dModel* opengl, OpenGL2dModel* draw, OpenGLJacobiModel* oglJacobi, struct_params* par,
        QWidget* parent = nullptr);
    ~DrawView();

public:
    void resetAll();
    void updateData();
    void updateParams();

    void adjustCoordNames();
    void adjustDrawTypes();
    void adjustEmbParams();
    void adjustLastPoint(unsigned int num = 0);

    int getDrawTypeIndex();
    int getDrawType3DIndex();
    std::string getDrawTypeName();
    void getDrawAbsOrdIndex(int& absIdx, int& ordIdx);
    std::string getDrawType3DName();

    void getColors(QColor& bgcol, QColor& fgcol, QColor& embcol);

    void animate(bool anim = false);
    bool isAnimated();

    void setGeodLength(int num);

    //void  addObjectsToScriptEngine(QScriptEngine* engine);

public slots:
    void setType(int num);
    void setPosition(double x, double y, double z);
    void setPOI(double x, double y, double z);
    void setFoV(double fov);

    void setScale(double sx, double sy, double sz);

    void setFog(int fog, double density = 0.0);

    void showNumPoints(int num);

protected slots:
    void slot_setDrawType();
    void slot_setAbsOrd();

    void slot_setScaling();
    void slot_resetScaling();
    void slot_centerScaling();
    void slot_adjustScaling();
    void slot_zoom();

    void slot_set2dFGcolor();
    void slot_set2dBGcolor();
    void slot_set2dGridColor();
    void slot_set2dLineWidth();

    void slot_setMouseHandle();

    void slot_setProjection();
    void slot_setFieldOfView();
    void slot_adjustCamera();
    void slot_setCameraPos();
    void slot_setCameraPoi();
    void slot_setCameraPredefs();
    void slot_resetPoi();

    void slot_setDrawType3d();

    void slot_setFGcolor();
    void slot_setBGcolor();
    void slot_setEmbColor();
    void slot_setLineWidth();
    void slot_setSmoothLine();

    void slot_set3dScaling();
    void slot_set3dScalingStep();
    void slot_set3dScalingReset();

    void slot_setStereo();
    void slot_setStereoParams();
    void slot_setStereoStep();
    void slot_setStereoType();

    void slot_setFog();
    void slot_setFogDensity();
    void slot_setFogDensityStep();

    void slot_anim_rot_startstop();
    void slot_anim_rotate();
    void slot_setAnimRotateParams();
    void slot_setAnimRotateParamsStep();

    void slot_embParamChanged();

    void slot_adjustAPname();
    void slot_showNumPoints(int num);

signals:
    void calcGeodesic();
    void colorChanged();
    void lastPointChanged(int num);

protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();
    void initStatusTips();

private:
    OpenGL3dModel* mOpenGL;
    OpenGL2dModel* mDraw;
    OpenGLJacobiModel* mOglJacobi;
    struct_params* mParams;

    QTabWidget* tab_draw;
    QWidget* wgt_draw_2d;
    QWidget* wgt_draw_3d;
    QWidget* wgt_draw_3dscale;
    QWidget* wgt_draw_3dstereo;
    QWidget* wgt_draw_3danim;
    QWidget* wgt_draw_3demb;

    // ----------------------
    //         2-D
    // ----------------------
    QComboBox* cob_drawtype;
    QLabel* lab_abscissa;
    QComboBox* cob_abscissa;
    QLabel* lab_ordinate;
    QComboBox* cob_ordinate;

    QLabel* lab_min;
    QLabel* lab_max;
    QLabel* lab_draw2d_abs;
    DoubleEdit* led_draw2d_x_min;
    DoubleEdit* led_draw2d_x_max;
    QLabel* lab_draw2d_ord;
    DoubleEdit* led_draw2d_y_min;
    DoubleEdit* led_draw2d_y_max;
    QPushButton* pub_draw2d_center;
    QPushButton* pub_draw2d_reset;
    QPushButton* pub_draw2d_zoom_in;
    QPushButton* pub_draw2d_zoom_out;

    QLabel* lab_draw2d_bgcolor;
    QPushButton* pub_draw2d_bgcolor;
    QLabel* lab_draw2d_fgcolor;
    QPushButton* pub_draw2d_fgcolor;
    QLabel* lab_draw2d_gridcolor;
    QPushButton* pub_draw2d_gridcolor;

    QLabel* lab_draw2d_linewidth;
    QSpinBox* spb_draw2d_linewidth;

    QLabel* lab_draw2d_pos;
    QLineEdit* led_draw2d_currX;
    QLineEdit* led_draw2d_currY;

    // ----------------------
    //         3-D
    // ----------------------
    QLabel* lab_projection;
    QComboBox* cob_projection;
    QLabel* lab_view;
    QComboBox* cob_view;
    QLabel* lab_bgcolor;
    QPushButton* pub_bgcolor;
    QLabel* lab_fgcolor;
    QPushButton* pub_fgcolor;
    QLabel* lab_linewidth;
    QSpinBox* spb_linewidth;
    QCheckBox* chb_linesmooth;
    QLabel* lab_drawtype3d;
    QComboBox* cob_drawtype3d;
    QLabel* lab_eye_x;
    DoubleEdit* led_eye_x;
    QLabel* lab_eye_y;
    DoubleEdit* led_eye_y;
    QLabel* lab_eye_z;
    DoubleEdit* led_eye_z;
    QLabel* lab_poi_x;
    DoubleEdit* led_poi_x;
    QLabel* lab_poi_y;
    DoubleEdit* led_poi_y;
    QLabel* lab_poi_z;
    DoubleEdit* led_poi_z;
    QLabel* lab_fov;
    DoubleEdit* led_fov;
    QPushButton* pub_reset_poi;

    // ----------------------
    //      3-D scale
    // ----------------------
    QLabel* lab_scale3d_value;
    QLabel* lab_scale3d_step;
    QLabel* lab_scale3d_x;
    DoubleEdit* led_scale3d_x;
    DoubleEdit* led_scale3d_x_step;
    QLabel* lab_scale3d_y;
    DoubleEdit* led_scale3d_y;
    DoubleEdit* led_scale3d_y_step;
    QLabel* lab_scale3d_z;
    DoubleEdit* led_scale3d_z;
    DoubleEdit* led_scale3d_z_step;
    QPushButton* pub_scale3d_reset;

    // ----------------------
    //      3-D stereo
    // ----------------------
    QCheckBox* chb_stereo;
    QComboBox* cob_glasses;
    QComboBox* cob_stereo_type;
    QLabel* lab_eye_sep;
    QLabel* lab_eye_sep_value;
    QLabel* lab_eye_sep_step;
    DoubleEdit* led_eye_sep;
    DoubleEdit* led_eye_sep_step;
    QCheckBox* chb_fog;
    QLabel* lab_fog_density;
    QLabel* lab_fog_value;
    QLabel* lab_fog_step;
    DoubleEdit* led_fog_density;
    DoubleEdit* led_fog_density_step;

    // ----------------------
    //      3-D animate
    // ----------------------
    QLabel* lab_anim_value;
    QLabel* lab_anim_step;
    QCheckBox* chb_anim_localrot;
    QLabel* lab_anim_rotate_x;
    DoubleEdit* led_anim_rotate_x;
    DoubleEdit* led_anim_rotate_x_step;
    QLabel* lab_anim_rotate_y;
    DoubleEdit* led_anim_rotate_y;
    DoubleEdit* led_anim_rotate_y_step;
    QLabel* lab_anim_rotate_z;
    DoubleEdit* led_anim_rotate_z;
    DoubleEdit* led_anim_rotate_z_step;
    QPushButton* pub_anim_rotate;
    QTimer* tim_anim_rotate;

    QSlider* sli_anim_geodlength;
    QLCDNumber* lcd_anim_geodlength;
    QLabel* lab_lastpoint_affineparam;
    QLineEdit* led_lastpoint_affineparam;
    QList<QLabel*> lst_lab_lastpoint_coordname;
    QList<QLineEdit*> lst_led_lastpoint_pos_value;

    // ----------------------
    //      3-D embedding
    // ----------------------
    QTableWidget* tbw_emb_params;
    QLabel* lab_emb_color;
    QPushButton* pub_emb_color;

    // ----------------------
    //    mouse handling
    // ----------------------
    QLabel* lab_mousehandle;
    QComboBox* cob_mousehandle;

    // ----------------------
    //    other things
    // ----------------------
    GreekLetter mGreekLetter;

    QColor mBGcolor;
    QColor mFGcolor;
    QColor mEmbColor;

    QColor m2dBGcolor;
    QColor m2dFGcolor;
    QColor m2dGridColor;
};

#endif
