// --------------------------------------------------------------------------------
/*
    geod_view.cpp

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

#include <QColorDialog>
#include <QGroupBox>
#include "geod_view.h"

extern m4d::Object mObject;

/*! Standard constructor.
 *  \param  data : pointer to Object storing all m4d.values.
 *  \param  par : pointer to struct.
 *  \param  opengl : pointer to opengl view.
 *  \param  openglJacobi : pointe to opengl Jacobi view.
 *  \param parent : pointer to parent widget.
 */
GeodView::GeodView(struct_params* par,
                   OpenGL3dModel*  opengl, OpenGLJacobiModel* openglJacobi, QWidget* parent)
    : QWidget(parent) {
    assert(opengl != NULL && openglJacobi != NULL);
    mParams  = par;

    mOpenGL      = opengl;
    mOglJacobi   = openglJacobi;
    mOrientation = enum_initdir_axis_3;
    init();
}

/*! Standard destructor.
 */
GeodView::~GeodView() {
}


// *********************************** public methods *********************************
/*!
 */
void GeodView::resetAll() {
    cob_geod_type->setCurrentIndex(0);
    led_geod_vel->setValueAndStep(0.99, 0.01);
    led_geod_vel->setEnabled(false);
    led_geod_vel_step->setValueAndStep(0.01, 0.001);
    led_geod_vel_step->setEnabled(false);
    cob_geod_timedir->setCurrentIndex(0);

    cps_ksi->reset();
    cps_ksi->setValue(0.0);
    cps_ksi->setStep(1.0);
    cps_chi->reset();
    cps_chi->setValue(90.0);
    cps_chi->setStep(1.0);
}

/*! Update date.
 *  Set the GUI elements with the data from mData.
 */
void GeodView::updateData() {
    if (mObject.timeDirection > 0) {
        cob_geod_timedir->setCurrentIndex(0);
    } else {
        cob_geod_timedir->setCurrentIndex(1);
    }

    cob_geod_type->setCurrentIndex((int)mObject.type);
    switch (mObject.type) {
        case m4d::enum_geodesic_lightlike:
            led_geod_vel->setEnabled(false);
            led_geod_vel_step->setEnabled(false);
            tab_geodesic->setTabEnabled(1, false);
            break;
        case m4d::enum_geodesic_lightlike_sachs:
            led_geod_vel->setEnabled(false);
            led_geod_vel_step->setEnabled(false);
            tab_geodesic->setTabEnabled(1, true);
            break;
        case m4d::enum_geodesic_timelike:
            led_geod_vel->setEnabled(true);
            led_geod_vel_step->setEnabled(true);
            tab_geodesic->setTabEnabled(1, false);
            break;
        case m4d::enum_geodesic_spacelike:
        default:
            break;
    }

    int ao = mObject.axes_orient;
    cob_orientation->setCurrentIndex(ao);
    mOrientation = (enum_initdir_orient)ao;

    double chi = mObject.chi;
    cps_ksi->setValue(mObject.ksi);
    mObject.chi = chi;
    cps_chi->setValue(mObject.chi);
    led_geod_vel->setText(QString::number(mObject.vel, 'f'));
    setInitDir();
}

/*!
 */
void GeodView::updateParams() {
    pub_sachs_leg1_col1->setPalette(QPalette(mParams->opengl_leg1_col1));
    pub_sachs_leg1_col2->setPalette(QPalette(mParams->opengl_leg1_col2));
    led_sachs_leg1_freq->setText(QString::number(mParams->opengl_leg1_freq));
    pub_sachs_leg2_col1->setPalette(QPalette(mParams->opengl_leg2_col1));
    pub_sachs_leg2_col2->setPalette(QPalette(mParams->opengl_leg2_col2));
    led_sachs_leg2_freq->setText(QString::number(mParams->opengl_leg2_freq));

    cob_sachs_orientation->setCurrentIndex((int)mParams->opengl_sachs_system);
    cob_sachs_legs->setCurrentIndex((int)mParams->opengl_sachs_legs);
    led_sachs_scale->setText(QString::number(mParams->opengl_sachs_scale));
}

/*! Add object to the script engine.
 * \param engine : pointer to master script engine.
 */
void GeodView::addObjectsToScriptEngine(QScriptEngine* engine) {
    QScriptValue ksiAngle = engine->newQObject(cps_ksi);
    engine->globalObject().setProperty("ksi", ksiAngle);

    QScriptValue chiAngle = engine->newQObject(cps_chi);
    engine->globalObject().setProperty("chi", chiAngle);

    QScriptValue geod = engine->newQObject(this);
    engine->globalObject().setProperty("geod", geod);
}


void GeodView::setDirection(double val, double ksi, double chi) {
    mObject.ksi = ksi;
    cps_ksi->setValue(mObject.ksi);
    mObject.chi = chi;
    cps_chi->setValue(mObject.chi);
    mObject.vel = val;
    led_geod_vel->setText(QString::number(mObject.vel, 'f'));
    setInitDir();
}

// ************************************* public slots ***********************************
/*!
 */
void GeodView::slot_showLastJacobi(int num) {
    m4d::vec5 last_jacobi;
    if (mObject.jacobi.size() == 0) {
        return;
    }

    if (num >= 0 && num <= (int)mObject.jacobi.size()) {
        last_jacobi  = mObject.jacobi[num];
        if (num >= (int)mObject.maxNumPoints) {
            last_jacobi = mObject.jacobi[mObject.maxNumPoints - 1];
        }

        led_jac_majorAxis->setText(QString::number(last_jacobi[0], 'e', DEF_PREC_JACOBI));
        led_jac_minorAxis->setText(QString::number(last_jacobi[1], 'e', DEF_PREC_JACOBI));
        led_jac_mu->setText(QString::number(last_jacobi[2], 'e', DEF_PREC_JACOBI));
        led_jac_angle->setText(QString::number(last_jacobi[3]*RAD_TO_DEG, 'f', DEF_PREC_JACOBI));
        led_jac_ellipt->setText(QString::number(last_jacobi[4], 'f', DEF_PREC_JACOBI));
    }
}

/*!
 */
void GeodView::slot_setLegColors() {
    QObject* obj = sender();
    QPushButton* pub = reinterpret_cast<QPushButton*>(obj);

    QColor oldCol = pub->palette().color(QPalette::Button);
    QColor newCol  = QColorDialog::getColor(oldCol);

    if (newCol.isValid()) {
        pub->setPalette(newCol);
        if (pub->objectName() == "leg1col1") {
            mParams->opengl_leg1_col1 = newCol;
        } else if (pub->objectName() == "leg1col2") {
            mParams->opengl_leg1_col2 = newCol;
        } else if (pub->objectName() == "leg2col1") {
            mParams->opengl_leg2_col1 = newCol;
        } else if (pub->objectName() == "leg2col2") {
            mParams->opengl_leg2_col2 = newCol;
        }

        mOpenGL->update();
    }
}

/*!
 */
void GeodView::slot_setLegFreq() {
    QObject* obj = sender();
    QLineEdit* led = reinterpret_cast<QLineEdit*>(obj);

    if (led->text() != QString()) {
        if (led->objectName() == "leg1freq") {
            mParams->opengl_leg1_freq = (float)led->text().toFloat();
        } else if (led->objectName() == "leg2freq") {
            mParams->opengl_leg2_freq = (float)led->text().toFloat();
        }

        mOpenGL->update();
    }
}

/*!
 */
void GeodView::slot_setSachsSystem(int num) {
    mParams->opengl_sachs_system = (enum_sachs_system)num;
    emit calcGeodesic();
}

/*!
 */
void GeodView::slot_setSachsLegs(int num) {
    mParams->opengl_sachs_legs = (enum_sachs_legs)num;
    mOpenGL->setSachsAxes(true);
}

/*!
 */
void GeodView::slot_setSachsScale() {
    mParams->opengl_sachs_scale = led_sachs_scale->text().toFloat();
    mOpenGL->setSachsAxes(true);
}

/*! Set geodesic type.
 *
 *  Use this function only from a script.
 * \param type : type of geodesic.
 */
void GeodView::setGeodesicType(int type) {
    if (type >= 0 && type < 3) {
        cob_geod_type->setCurrentIndex(type);
        slot_setGeodType();
    }
}

/*! Set geodesic type.
 *
 *  Use this function only from a script.
 * \param name : name of geodesic type.
 */
void GeodView::setGeodesicType(QString name) {
    int num = 0;
    while (num < (int)m4d::NUM_ENUM_GEODESIC_TYPE - 1) {
        if (name.compare(QString(m4d::stl_geodesic_type[num])) == 0) {
            cob_geod_type->setCurrentIndex(num);
            slot_setGeodType();
            break;
        }
        num++;
    }
}

/*! Set time direction for geodesic.
 *
 *  Use this function only from a script.
 * \param dir : time direction: +1:future, -1:past
 */
void GeodView::setTimeDirection(int dir) {
    if (dir == 1) {
        cob_geod_timedir->setCurrentIndex(0);
        mObject.timeDirection = 1;
        emit calcGeodesic();
    } else if (dir == -1) {
        cob_geod_timedir->setCurrentIndex(1);
        mObject.timeDirection = -1;
        emit calcGeodesic();
    } else {
        fprintf(stderr, "GeodView::setTimeDirection() ... direction %d not valid!\nUse +1 or -1.\n", dir);
    }
}

/*! Set initial velocity for timelike geodesic.
 *
 *  Use this function only from a script.
 * \param val : velocity scaled by c.
 */
void GeodView::setVelocity(double val) {
    led_geod_vel->setValue(val);
    slot_setVelocity();
}

// ************************************* protected slots ***********************************
/*! Set time direction of geodesic.
 */
void GeodView::slot_setTimeDirection() {
    int index = cob_geod_timedir->currentIndex();
    if (index == 0) {
        mObject.timeDirection = 1;
    } else {
        mObject.timeDirection = -1;
    }
    emit calcGeodesic();
}

/*! Set geodesic type.
 */
void GeodView::slot_setGeodType() {
    if (mObject.geodSolver == NULL) {
        return;
    }

    int index = cob_geod_type->currentIndex();
    switch (index) {
        case m4d::enum_geodesic_lightlike: {
            mObject.type = m4d::enum_geodesic_lightlike;
            mObject.geodSolver->setGeodesicType(mObject.type);
            led_geod_vel->setText(QString::number(mObject.vel, 'f'));
            led_geod_vel->setEnabled(false);
            led_geod_vel_step->setEnabled(false);
            tab_geodesic->setTabEnabled(1, false);
        }
        break;
        case m4d::enum_geodesic_lightlike_sachs: {
            mObject.type = m4d::enum_geodesic_lightlike_sachs;
            mObject.geodSolver->setGeodesicType(mObject.type);
            led_geod_vel->setText(QString::number(mObject.vel, 'f'));
            led_geod_vel->setEnabled(false);
            led_geod_vel_step->setEnabled(false);
            tab_geodesic->setTabEnabled(1, true);
        }
        break;
        case m4d::enum_geodesic_timelike: {
            mObject.type = m4d::enum_geodesic_timelike;
            mObject.geodSolver->setGeodesicType(mObject.type);
            mObject.vel = led_geod_vel->getValue();
            led_geod_vel->setEnabled(true);
            led_geod_vel_step->setEnabled(true);
            tab_geodesic->setTabEnabled(1, false);
        }
        break;
    }
    emit calcGeodesic();
    emit changeGeodType();
}

/*! Set direction.
 */
void GeodView::slot_setDirection() {
    mObject.ksi = cps_ksi->value();
    mObject.chi = cps_chi->value();

    setInitDir();
    emit calcGeodesic();
}

/*! Set velocity.
 */
void GeodView::slot_setVelocity() {
    double newVel = led_geod_vel->getValue();

    if (1.0 - fabs(newVel) < 1e-10) {
        led_geod_vel->setValue(mObject.vel);
    } else {
        mObject.vel = newVel;
    }

    setInitDir();
    emit calcGeodesic();
}

/*! Set velocity step.
 */
void GeodView::slot_setVelocityStep() {
    led_geod_vel->setStep(led_geod_vel_step->getValue());
}

/*! Set orientation of the initial direction angles.
 */
void GeodView::slot_setOrientation() {
    mObject.axes_orient = cob_orientation->currentIndex();
    mOrientation = (enum_initdir_orient)mObject.axes_orient;

    slot_setDirection();
}

/*!
 */
void GeodView::slot_incrKsi() {
    cps_ksi->doStep(1);
    slot_setDirection();
}

/*!
 */
void GeodView::slot_decrKsi() {
    cps_ksi->doStep(-1);
    slot_setDirection();
}

/*!
 */
void GeodView::slot_incrChi() {
    cps_chi->doStep(1);
    slot_setDirection();
}

/*!
 */
void GeodView::slot_decrChi() {
    cps_chi->doStep(-1);
    slot_setDirection();
}

/*!
 */
void GeodView::slot_setShowJacobi() {
    QObject* obj = sender();
    int num = obj->objectName().toInt();

    mOglJacobi->setActualJacobi((enum_jacobi_param)num);
}

// *********************************** protected methods *********************************
/*! Initialize view.
 */
void GeodView::init() {
    initElements();
    initGUI();
    initControl();

    initStatusTips();
}

/*! Initialize view elements.
 */
void GeodView::initElements() {
    lab_value = new QLabel("value");
    lab_step  = new QLabel("step");

    lab_geod_type = new QLabel(tr("type"));
    lab_geod_type->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    cob_geod_type = new QComboBox();
    for (unsigned int i = 0; i < m4d::NUM_ENUM_GEODESIC_TYPE - 1; i++) {
        cob_geod_type->addItem(m4d::stl_geodesic_type[i]);
    }
    cob_geod_type->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    lab_geod_vel  = new QLabel(tr("velocity ") + QString(mGreekLetter.toChar("beta")));
    led_geod_vel  = new DoubleEdit(DEF_PREC_VELOCITY, 0.99, 0.01);
    led_geod_vel->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_geod_vel->setEnabled(false);
    lab_geod_vel_step = new QLabel(tr("step"));
    led_geod_vel_step = new DoubleEdit(DEF_PREC_VELOCITY, 0.01, 0.001);
    led_geod_vel_step->setMaximumSize(DEF_MAXIMUM_LE_S_WIDTH, DEF_MAXIMUM_ELEM_HEIGHT);
    led_geod_vel_step->setEnabled(false);
    lab_geod_timedir = new QLabel(tr("time dir"));
    lab_geod_timedir->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    cob_geod_timedir = new QComboBox();
    cob_geod_timedir->addItem("+1 (future)");
    cob_geod_timedir->addItem("-1 (past)");
    cob_geod_timedir->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    cps_ksi = new CompassDial("xi", this);
    cps_ksi->setRange(360.0);
    cps_ksi->setStep(1.0);

    cps_chi = new CompassDial("chi", this);
    cps_chi->setRange(180.0);
    cps_chi->setValue(90.0);
    cps_chi->setInitValue(90.0);
    cps_chi->setStep(1.0);

    lab_orientation = new QLabel(tr("Orientation"));
    lab_orientation->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    cob_orientation = new QComboBox();
    cob_orientation->addItems(stl_initdir_orient);
    cob_orientation->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    QHBoxLayout* layout_orient = new QHBoxLayout();
    layout_orient -> addWidget(lab_orientation);
    layout_orient -> addWidget(cob_orientation);

    wgt_geod_angles = new QWidget();
    QGridLayout*  layout_geod_angles = new QGridLayout();
    layout_geod_angles -> addLayout(layout_orient, 0, 0);
    layout_geod_angles -> addWidget(cps_ksi, 1, 0, Qt::AlignHCenter);
    layout_geod_angles -> addWidget(cps_chi, 1, 1, Qt::AlignHCenter);
    wgt_geod_angles->setLayout(layout_geod_angles);


    /* -----------------------------------
        Sachs axis
     ----------------------------------- */
    lab_sachs_orientation = new QLabel("System:");
    cob_sachs_orientation = new QComboBox();
    cob_sachs_orientation->addItems(stl_sachs_system);
    cob_sachs_orientation->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    lab_sachs_legs = new QLabel("Legs:");
    cob_sachs_legs = new QComboBox();
    cob_sachs_legs->addItems(stl_sachs_legs);
    cob_sachs_legs->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_sachs_leg1      = new QLabel("leg1:");
    pub_sachs_leg1_col1 = new QPushButton();
    pub_sachs_leg1_col1->setObjectName("leg1col1");
    pub_sachs_leg1_col1->setPalette(QPalette(mParams->opengl_leg1_col1));
    pub_sachs_leg1_col1->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_sachs_leg1_col2 = new QPushButton();
    pub_sachs_leg1_col2->setObjectName("leg1col2");
    pub_sachs_leg1_col2->setPalette(QPalette(mParams->opengl_leg1_col2));
    pub_sachs_leg1_col2->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_sachs_leg1_freq = new QLineEdit(QString::number(mParams->opengl_leg1_freq));
    led_sachs_leg1_freq->setValidator(new QDoubleValidator(led_sachs_leg1_freq));
    led_sachs_leg1_freq->setObjectName("leg1freq");
    led_sachs_leg1_freq->setAlignment(Qt::AlignRight);
    led_sachs_leg1_freq->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_sachs_leg1_freq->setMaximumWidth(80);

    lab_sachs_leg2      = new QLabel("leg2:");
    pub_sachs_leg2_col1 = new QPushButton();
    pub_sachs_leg2_col1->setObjectName("leg2col1");
    pub_sachs_leg2_col1->setPalette(QPalette(mParams->opengl_leg2_col1));
    pub_sachs_leg2_col1->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    pub_sachs_leg2_col2 = new QPushButton();
    pub_sachs_leg2_col2->setObjectName("leg2col2");
    pub_sachs_leg2_col2->setPalette(QPalette(mParams->opengl_leg2_col2));
    pub_sachs_leg2_col2->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_sachs_leg2_freq = new QLineEdit(QString::number(mParams->opengl_leg1_freq));
    led_sachs_leg2_freq->setValidator(new QDoubleValidator(led_sachs_leg2_freq));
    led_sachs_leg2_freq->setObjectName("leg2freq");
    led_sachs_leg2_freq->setAlignment(Qt::AlignRight);
    led_sachs_leg2_freq->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_sachs_leg2_freq->setMaximumWidth(80);

    lab_sachs_scale    = new QLabel("Scale:");
    led_sachs_scale    = new QLineEdit(QString::number(mParams->opengl_sachs_scale));
    led_sachs_scale->setValidator(new QDoubleValidator(led_sachs_scale));
    led_sachs_scale->setAlignment(Qt::AlignRight);
    led_sachs_scale->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    led_sachs_scale->setMaximumWidth(80);

    QGroupBox* grb_sachs = new QGroupBox("Sachs basis");
    QGridLayout* layout_sachs = new QGridLayout();
    layout_sachs -> addWidget(lab_sachs_orientation, 0, 0);
    layout_sachs -> addWidget(cob_sachs_orientation, 0, 1);
    layout_sachs -> addWidget(lab_sachs_legs, 1, 0);
    layout_sachs -> addWidget(cob_sachs_legs, 1, 1);
    layout_sachs -> addWidget(lab_sachs_leg1,        0, 2);
    layout_sachs -> addWidget(pub_sachs_leg1_col1,   0, 3);
    layout_sachs -> addWidget(pub_sachs_leg1_col2,   0, 4);
    layout_sachs -> addWidget(led_sachs_leg1_freq,   0, 5);
    layout_sachs -> addWidget(lab_sachs_leg2,        1, 2);
    layout_sachs -> addWidget(pub_sachs_leg2_col1,   1, 3);
    layout_sachs -> addWidget(pub_sachs_leg2_col2,   1, 4);
    layout_sachs -> addWidget(led_sachs_leg2_freq,   1, 5);
    layout_sachs -> addWidget(lab_sachs_scale,       2, 0);
    layout_sachs -> addWidget(led_sachs_scale,       2, 1);
    grb_sachs->setLayout(layout_sachs);

    /* -----------------------------------
        Jacobi parameter
     ----------------------------------- */
    rab_jac_ellipse   = new QRadioButton("Ellipse");
    rab_jac_ellipse->setObjectName(QString::number((int)enum_jacobi_ellipse));
    rab_jac_ellipse->setChecked(true);
    rab_jac_majorAxis = new QRadioButton("MajAxis");
    rab_jac_majorAxis->setObjectName(QString::number((int)enum_jacobi_dp));
    led_jac_majorAxis = new QLineEdit();
    led_jac_majorAxis->setReadOnly(true);
    led_jac_majorAxis->setAlignment(Qt::AlignRight);
    led_jac_majorAxis->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    rab_jac_minorAxis = new QRadioButton("MinAxis");
    rab_jac_minorAxis->setObjectName(QString::number((int)enum_jacobi_dm));
    led_jac_minorAxis = new QLineEdit();
    led_jac_minorAxis->setAlignment(Qt::AlignRight);
    led_jac_minorAxis->setReadOnly(true);
    led_jac_minorAxis->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    rab_jac_mu        = new QRadioButton("mu");
    rab_jac_mu->setObjectName(QString::number((int)enum_jacobi_mu));
    led_jac_mu        = new QLineEdit();
    led_jac_mu->setAlignment(Qt::AlignRight);
    led_jac_mu->setReadOnly(true);
    led_jac_mu->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    rab_jac_angle     = new QRadioButton("angle");
    rab_jac_angle->setObjectName(QString::number((int)enum_jacobi_angle));
    led_jac_angle     = new QLineEdit();
    led_jac_angle->setAlignment(Qt::AlignRight);
    led_jac_angle->setReadOnly(true);
    led_jac_angle->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
    rab_jac_ellipt    = new QRadioButton("ellipt");
    rab_jac_ellipt->setObjectName(QString::number((int)enum_jacobi_ellipt));
    led_jac_ellipt    = new QLineEdit();
    led_jac_ellipt->setAlignment(Qt::AlignRight);
    led_jac_ellipt->setReadOnly(true);
    led_jac_ellipt->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    QGroupBox* grb_jacobi = new QGroupBox("Jacobi parameters");
    QGridLayout* layout_jacobi = new QGridLayout();
    layout_jacobi -> addWidget(mOglJacobi, 0, 0, 6, 1);
    layout_jacobi -> addWidget(rab_jac_ellipse,   0, 1);
    layout_jacobi -> addWidget(rab_jac_majorAxis, 1, 1);
    layout_jacobi -> addWidget(led_jac_majorAxis, 1, 2);
    layout_jacobi -> addWidget(rab_jac_minorAxis, 2, 1);
    layout_jacobi -> addWidget(led_jac_minorAxis, 2, 2);
    layout_jacobi -> addWidget(rab_jac_mu,        3, 1);
    layout_jacobi -> addWidget(led_jac_mu,        3, 2);
    layout_jacobi -> addWidget(rab_jac_angle,     4, 1);
    layout_jacobi -> addWidget(led_jac_angle,     4, 2);
    layout_jacobi -> addWidget(rab_jac_ellipt,    5, 1);
    layout_jacobi -> addWidget(led_jac_ellipt,    5, 2);
    grb_jacobi->setLayout(layout_jacobi);

    wgt_sachs_jacobi = new QWidget();
    QVBoxLayout* layout_sachs_jacobi = new QVBoxLayout();
    layout_sachs_jacobi -> addWidget(grb_sachs);
    layout_sachs_jacobi -> addWidget(grb_jacobi);
    wgt_sachs_jacobi->setLayout(layout_sachs_jacobi);

    tab_geodesic = new QTabWidget;
    tab_geodesic -> addTab(wgt_geod_angles,  tr("Direction Angles"));
    tab_geodesic -> addTab(wgt_sachs_jacobi, tr("Sachs/Jacobi"));
    tab_geodesic->setTabEnabled(1, false);
}

/*! Construct graphical user interface.
 */
void GeodView::initGUI() {
    /* ---------------------------------
        layout geodesic
     --------------------------------- */
    QGroupBox*  grb_geodesic = new QGroupBox("Geodesic");
    QGridLayout* layout_geodesic = new QGridLayout();
    layout_geodesic -> addWidget(lab_geod_type, 0, 0);
    layout_geodesic -> addWidget(cob_geod_type, 0, 1);
    layout_geodesic -> addWidget(lab_geod_timedir, 0, 2);
    layout_geodesic -> addWidget(cob_geod_timedir, 0, 3);
    layout_geodesic -> addWidget(lab_geod_vel,  1, 0);
    layout_geodesic -> addWidget(led_geod_vel,  1, 1);
    layout_geodesic -> addWidget(lab_geod_vel_step, 1, 2);
    layout_geodesic -> addWidget(led_geod_vel_step, 1, 3);
    layout_geodesic -> addWidget(tab_geodesic, 2, 0, 1, 4);
    grb_geodesic->setLayout(layout_geodesic);

    QVBoxLayout* layout_complete = new QVBoxLayout();
    layout_complete->addWidget(grb_geodesic);
    setLayout(layout_complete);

    this->setMinimumWidth(DEF_GROUPBOX_WIDTH);
    this->setMaximumWidth(DEF_GROUPBOX_WIDTH);
}

/*! Connect signals and slots.
 */
void GeodView::initControl() {
    connect(cob_geod_type,     SIGNAL(activated(int)), this, SLOT(slot_setGeodType()));
    connect(cob_geod_timedir,  SIGNAL(activated(int)), this, SLOT(slot_setTimeDirection()));
    connect(led_geod_vel,      SIGNAL(editingFinished()), this, SLOT(slot_setVelocity()));
    connect(led_geod_vel_step, SIGNAL(editingFinished()), this, SLOT(slot_setVelocityStep()));

    connect(cps_ksi, SIGNAL(compassDialValueChanged(double)), this, SLOT(slot_setDirection()));
    connect(cps_chi, SIGNAL(compassDialValueChanged(double)), this, SLOT(slot_setDirection()));

    connect(cob_orientation, SIGNAL(activated(int)), this, SLOT(slot_setOrientation()));

    connect(pub_sachs_leg1_col1, SIGNAL(pressed()), this, SLOT(slot_setLegColors()));
    connect(pub_sachs_leg1_col2, SIGNAL(pressed()), this, SLOT(slot_setLegColors()));
    connect(pub_sachs_leg2_col1, SIGNAL(pressed()), this, SLOT(slot_setLegColors()));
    connect(pub_sachs_leg2_col2, SIGNAL(pressed()), this, SLOT(slot_setLegColors()));

    connect(cob_sachs_orientation, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setSachsSystem(int)));
    connect(cob_sachs_legs,      SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setSachsLegs(int)));
    connect(led_sachs_leg1_freq, SIGNAL(editingFinished()), this, SLOT(slot_setLegFreq()));
    connect(led_sachs_leg2_freq, SIGNAL(editingFinished()), this, SLOT(slot_setLegFreq()));
    connect(led_sachs_scale,     SIGNAL(editingFinished()), this, SLOT(slot_setSachsScale()));

    connect(rab_jac_ellipse,   SIGNAL(pressed()), this, SLOT(slot_setShowJacobi()));
    connect(rab_jac_majorAxis, SIGNAL(pressed()), this, SLOT(slot_setShowJacobi()));
    connect(rab_jac_minorAxis, SIGNAL(pressed()), this, SLOT(slot_setShowJacobi()));
    connect(rab_jac_mu,        SIGNAL(pressed()), this, SLOT(slot_setShowJacobi()));
    connect(rab_jac_angle,     SIGNAL(pressed()), this, SLOT(slot_setShowJacobi()));
    connect(rab_jac_ellipt,    SIGNAL(pressed()), this, SLOT(slot_setShowJacobi()));
}


/*! Initialize status tips.
 */
void GeodView::initStatusTips() {
#ifdef SHOW_STATUS_TIPS
    cob_geod_type     -> setStatusTip(tr("Select type of geodesic."));
    led_geod_vel      -> setStatusTip(tr("Initial velocity for time-like geodesic."));
    led_geod_vel_step -> setStatusTip(tr("Step size for initial velocity."));
    cob_geod_timedir  -> setStatusTip(tr("Geodesic is future or past directed."));

    cob_orientation   -> setStatusTip(tr("Orientation of the local coordinate system."));
    cps_ksi           -> setStatusTip(tr("Initial longitudinal angle (deg)."));
    cps_chi           -> setStatusTip(tr("Initial colatitudinal angle (deg)."));

    cob_sachs_orientation -> setStatusTip(tr("Orientation of the Sachs basis."));
    cob_sachs_legs        -> setStatusTip(tr("Sachs legs to be shown."));
    led_sachs_leg1_freq   -> setStatusTip(tr("Frequency of the stripe pattern for Sachs basis 1."));
    led_sachs_leg2_freq   -> setStatusTip(tr("Frequency of the stripe pattern for Sachs basis 2."));
    led_sachs_scale       -> setStatusTip(tr("Scale factor for the Sachs legs."));
#endif
}


/*! Set initial direction.
 */
void GeodView::setInitDir() {
    switch (mOrientation) {
        default:
        case enum_initdir_axis_3: {
            mObject.startDir[0] = sin(mObject.chi * DEG_TO_RAD) * cos(mObject.ksi * DEG_TO_RAD);
            mObject.startDir[1] = sin(mObject.chi * DEG_TO_RAD) * sin(mObject.ksi * DEG_TO_RAD);
            mObject.startDir[2] = cos(mObject.chi * DEG_TO_RAD);
            break;
        }
        case enum_initdir_axis_1: {
            mObject.startDir[0] = cos(mObject.chi * DEG_TO_RAD);
            mObject.startDir[1] = sin(mObject.chi * DEG_TO_RAD) * cos(mObject.ksi * DEG_TO_RAD);
            mObject.startDir[2] = sin(mObject.chi * DEG_TO_RAD) * sin(mObject.ksi * DEG_TO_RAD);
            break;
        }
        case enum_initdir_axis_2: {
            mObject.startDir[0] = sin(mObject.chi * DEG_TO_RAD) * cos(mObject.ksi * DEG_TO_RAD);
            mObject.startDir[1] = cos(mObject.chi * DEG_TO_RAD);
            mObject.startDir[2] = sin(mObject.chi * DEG_TO_RAD) * sin(mObject.ksi * DEG_TO_RAD);
            break;
        }
    }
}
