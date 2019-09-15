/**
 * @file    geod_view.h
 * @author  Thomas Mueller
 *
 * @brief  iew for geodesic manipulation.

        The initial direction of a geodesic is defined with respect to the local
        reference frame given in the LocalTetrad window. With the initial
        angles &xi; and %chi; the four-vector of a null geodesic reads
        \f[ \mathbf{k} = \mathbf{e}_{(0)} + \sin\chi\cos\xi\mathbf{e}_{(1)} + \sin\chi\sin\xi\mathbf{e}_{(2)} + \cos\chi\mathbf{e}_{(3)}.\f]
     <img src="../pics/geodesic.png"> \sa lab_geod_timedir
 *
 * This file is part of GeodesicView.
 */
#ifndef GEOD_VIEW_H
#define GEOD_VIEW_H

#include <iostream>

#include <QComboBox>
#include <QJSEngine>
#include <QLabel>
#include <QRadioButton>
#include <QTabWidget>

#include <compass_dial.h>

#include <doubleedit_util.h>
#include <gdefs.h>
#include <greek.h>
#include <model/opengl3d_model.h>
#include <model/openglJacobi_model.h>

#include <extra/m4dObject.h>
#include <m4dGlobalDefs.h>

class GeodView : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Standard constructor.
     * @param par
     * @param opengl
     * @param openglJacobi
     * @param parent
     */
    GeodView(struct_params* par,
        OpenGL3dModel* opengl, OpenGLJacobiModel* openglJacobi, QWidget* parent = nullptr);
    ~GeodView();

public:
    //void  addObjectsToScriptEngine(QScriptEngine* engine);
    void resetAll();
    void updateData();
    void updateParams();

    void setDirection(double val, double ksi, double chi);

public slots:
    void slot_showLastJacobi(int num);
    void slot_setLegColors();
    void slot_setLegFreq();
    void slot_setSachsSystem(int num);
    void slot_setSachsLegs(int num);
    void slot_setSachsScale();

    // script functions
    void setGeodesicType(int type);
    void setGeodesicType(QString name);
    void setTimeDirection(int dir);
    void setVelocity(double val);

signals:
    void calcGeodesic();
    void changeGeodType();

protected slots:
    void slot_setTimeDirection();
    void slot_setGeodType();
    void slot_setDirection();
    void slot_setVelocity();
    void slot_setVelocityStep();
    void slot_setOrientation();

    void slot_incrKsi();
    void slot_decrKsi();
    void slot_incrChi();
    void slot_decrChi();

    void slot_setShowJacobi();

protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();
    void initStatusTips();

    void setInitDir();

private:
    struct_params* mParams;
    QLabel* lab_value;
    QLabel* lab_step;

    QTabWidget* tab_geodesic;
    QWidget* wgt_geod_angles;
    QWidget* wgt_sachs_jacobi;

    QLabel* lab_geod_type;
    QComboBox* cob_geod_type;
    QLabel* lab_geod_vel;
    DoubleEdit* led_geod_vel;
    QLabel* lab_geod_vel_step;
    DoubleEdit* led_geod_vel_step;
    QLabel* lab_geod_timedir;
    QComboBox* cob_geod_timedir;

    QLabel* lab_orientation;
    QComboBox* cob_orientation;

    CompassDial* cps_ksi;
    CompassDial* cps_chi;

    // Sachsbasis
    QComboBox* cob_sachs_orientation;
    QComboBox* cob_sachs_legs;
    QPushButton* pub_sachs_leg1_col1;
    QPushButton* pub_sachs_leg1_col2;
    QLineEdit* led_sachs_leg1_freq;
    QPushButton* pub_sachs_leg2_col1;
    QPushButton* pub_sachs_leg2_col2;
    QLineEdit* led_sachs_leg2_freq;
    QLineEdit* led_sachs_scale;

    // Jacobi
    QRadioButton* rab_jac_ellipse;
    QRadioButton* rab_jac_majorAxis;
    QLineEdit* led_jac_majorAxis;
    QRadioButton* rab_jac_minorAxis;
    QLineEdit* led_jac_minorAxis;
    QRadioButton* rab_jac_mu;
    QLineEdit* led_jac_mu;
    QRadioButton* rab_jac_angle;
    QLineEdit* led_jac_angle;
    QRadioButton* rab_jac_ellipt;
    QLineEdit* led_jac_ellipt;

    enum_initdir_orient mOrientation;
    GreekLetter mGreekLetter;

    OpenGL3dModel* mOpenGL;
    OpenGLJacobiModel* mOglJacobi;
};

#endif
