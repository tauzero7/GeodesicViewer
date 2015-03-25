// --------------------------------------------------------------------------------
/*
    doubleedit_util.cpp

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

#include <QWheelEvent>
#include "doubleedit_util.h"

/*! Standard constructor.
 *  \param prec  : precision.
 *  \param value : initial value.
 *  \param step  : stepsize.
 *  \param parent : parent widget.
 */
DoubleEdit::DoubleEdit(int prec, double value, double step, QWidget* parent)
    : QLineEdit(parent) {
    mPrecision = prec;
    mStep = step;
    setValue(value);
    setAlignment(Qt::AlignRight);

    mMin = -DBL_MAX;
    mMax = DBL_MAX;

    setPalette(QPalette(QColor(DEF_DOUBLE_EDIT_COLOR)));
}

/*! Standard destructor.
 */
DoubleEdit::~DoubleEdit() {
}


// *********************************** public methods ******************************
/*! Set value.
 *  \param value : new value.
 */
void
DoubleEdit::setValue(double value) {
    setText(QString::number(value, 'f', mPrecision));
}

/*! Get value.
 *  \return  value.
 */
double
DoubleEdit::getValue() {
    /*
    double val = atof(text().toStdString().c_str());
    fprintf(stderr,"%15.12f  %15.12f\n",val,text().toDouble());
    */
    return text().toDouble();
}

/*! Set stepsize.
 *  \param step : new stepsize.
 */
void
DoubleEdit::setStep(double step) {
    mStep = step;
}

/*! Get stepsize.
 *  \return  stepsize.
 */
double
DoubleEdit::getStep() {
    return mStep;
}

/*! Set value and stepsize.
 *  \param value : new value.
 *  \param step  : new step;
 */
void
DoubleEdit::setValueAndStep(double value, double step) {
    setValue(value);
    setStep(step);
}

/*! Get value and stepsize.
 *  \param value : reference to value.
 *  \param step  : reference to step;
 */
void
DoubleEdit::getValueAndStep(double &value, double &step) {
    value = getValue();
    step  = getStep();
}

/*! Set range for input.
 *  \param min : range minimum.
 *  \param max : range maximum.
 */
void
DoubleEdit::setRange(double min, double max) {
    mMin = min;
    mMax = max;
}

/*! Get range for input.
 *  \param min : reference to range minimum.
 *  \param max : reference to range maximum.
 */
void
DoubleEdit::getRange(double &min, double &max) {
    min = mMin;
    max = mMax;
}

// *********************************** public slots ********************************
/*!
 */
void
DoubleEdit::slot_setValue() {
    QObject* obj = sender();
    DoubleEdit* led = reinterpret_cast<DoubleEdit*>(obj);
    double value = led->getValue();
    setValue(value);
}

/*!
 */
void
DoubleEdit::slot_setStep() {
    QObject* obj = sender();
    DoubleEdit* led = reinterpret_cast<DoubleEdit*>(obj);
    double value = led->getValue();
    setStep(value);
}

// ********************************* protected methods *****************************
/*!  Wheel event handling.
 * \param event : pointer to QWheelEvent.
 */
void
DoubleEdit::wheelEvent(QWheelEvent* event) {
    double val = text().toDouble();

    if (event->delta() > 0) {
        val += mStep;
    } else {
        val -= mStep;
    }

    if (val > mMax) {
        val = mMax;
    }
    if (val < mMin) {
        val = mMin;
    }

    setText(QString("%1").arg(val, 0, 'f', mPrecision));
    event->accept();

    //emit returnPressed();
    emit editingFinished();
}
