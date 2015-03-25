// --------------------------------------------------------------------------------
/*
    compass_dial.cpp

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

#include "compass_dial.h"

/*! Constructor of the CompassDial
 * \param valName : name of value
 * \param parent : pointer to parent widget
 */
CompassDial::CompassDial(QString valName, QWidget* parent)
    : QWidget(parent) {
    if (valName == QString()) {
        mValueName = QString("value");
    } else {
        QChar ch = mGreekLetter.toChar(valName);
        if (ch == QChar()) {
            mValueName = valName;
        } else {
            mValueName = ch;
        }
    }

    mInitValue = 0.0;
    mValue = mStep = 0.0;
    init();
}

/*!
 */
CompassDial::~CompassDial() {
}


// *********************************** public methods *********************************

/*!
 */
void CompassDial::reset() {
    mValue = mInitValue;
    led_param_val->setValue(mInitValue);
    led_param_step->setValue(DEF_COMPASS_DIAL_STEP);
    emit compassDialValueChanged(mInitValue);
}

/*! Set compass range.
 * \param valMax : maximum value
 */
void CompassDial::setRange(double valMax) {
    dial->setMax(valMax);
}

/*! Set value.
 * \param val : value
 */
void CompassDial::setValue(double val) {
    mValue = val;
    dial->setValue(val);
    led_param_val->setText(QString::number(val, 'f'));
}

/*! Set step.
 * \param step : step value
 */
void CompassDial::setStep(double step) {
    mStep = step;
    dial->setStep(step);
    led_param_step->setText(QString::number(step, 'f'));
}

/*! Get value.
 * \return value
 */
double
CompassDial::value() {
    return mValue;
}

void CompassDial::doStep(int step) {
    setValue(mValue + step * mStep);
}

/*! Set initial value.
 * \param val : initial value
 */
void CompassDial::setInitValue(double val) {
    mInitValue = val;
}

// *********************************** protected methods *********************************

/*! Initialize view.
 */
void CompassDial::init() {
    initElements();
    initGUI();
    initControl();

}

/*! Initialize view elements.
 */
void CompassDial::initElements() {
    QPalette colorPal;
    colorPal.setColor(QPalette::Base, QColor(20, 20, 20));
    colorPal.setColor(QPalette::Foreground, QColor(50, 50, 50));
    colorPal.setColor(QPalette::Text, Qt::white);

    dial = new DialWidget(0.0, 1.0, this);
    dial->setMinimumSize(200, 200);

    QMap<double, QString> map;
    for (double d = 0.0; d < 360.0; d += 60.0) {
        QString label;
        label.sprintf("%.0f", d);
        map.insert(d, label);
    }

    lab_param_val  = new QLabel(mValueName);
    led_param_val  = new DoubleEdit(DEF_PREC_COMPASS, 0.0, DEF_COMPASS_DIAL_STEP);
    led_param_val->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
//  led_param_val->setValidator( new QDoubleValidator( led_param_val ) );
    led_param_val->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    lab_param_step = new QLabel("step");
    led_param_step = new DoubleEdit(DEF_PREC_COMPASS, DEF_COMPASS_DIAL_STEP);
    led_param_step->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
//  led_param_step->setValidator( new QDoubleValidator( led_param_step ) );
    led_param_step->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);

    pub_reset = new QPushButton("reset");
    pub_reset->setMaximumHeight(DEF_MAXIMUM_ELEM_HEIGHT);
}

/*! Construct graphical user interface.
 */
void CompassDial::initGUI() {
    QGridLayout* layout_complete = new QGridLayout();
    layout_complete -> addWidget(dial, 0, 0, 1, 2);
    layout_complete -> addWidget(lab_param_val, 1, 0);
    layout_complete -> addWidget(led_param_val, 1, 1);
    layout_complete -> addWidget(lab_param_step, 2, 0);
    layout_complete -> addWidget(led_param_step, 2, 1);
    layout_complete -> addWidget(pub_reset, 3, 0, 1, 2);
    setLayout(layout_complete);
}

/*! Connect signals and slots.
 */
void CompassDial::initControl() {
    connect(dial, SIGNAL(dialValueChanged(double)), this, SLOT(set(double)));
    connect(led_param_val,  SIGNAL(editingFinished()),  this, SLOT(slot_paramChanged()));
    connect(led_param_step, SIGNAL(editingFinished()),  this, SLOT(slot_stepChanged()));
    connect(pub_reset,      SIGNAL(pressed()), this, SLOT(slot_reset()));

    connect(led_param_val,  SIGNAL(returnPressed()), this, SLOT(slot_paramChanged()));
    connect(led_param_step, SIGNAL(returnPressed()),  this, SLOT(slot_stepChanged()));
}

// *********************************** public slots *********************************

/*! Value changed.
 *  \param val : value
 */
void CompassDial::set(double val) {
    mValue = val;
    led_param_val->setValue(val);
    dial->setValue(mValue);
    emit compassDialValueChanged(val);
}

// *********************************** protected slots *********************************

/*! Step changed.
 */
void CompassDial::slot_stepChanged() {
    mStep = led_param_step->getValue();
    led_param_val->setStep(mStep);
    dial->setStep(mStep);
}

/*! Parameter changed.
 */
void CompassDial::slot_paramChanged() {
    mValue = led_param_val->getValue();
    dial->setValue(mValue);
    emit compassDialValueChanged(mValue);
}

/*! Reset
 */
void CompassDial::slot_reset() {
    mValue = mInitValue;
    dial->setInitialValue(mInitValue);
    dial->reset();
    led_param_val->setValue(mInitValue);
    emit compassDialValueChanged(mInitValue);
}
