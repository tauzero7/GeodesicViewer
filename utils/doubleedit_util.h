// --------------------------------------------------------------------------------
/*
    doubleedit_util.h

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

/*!  \class  DoubleEdit
     \brief  Line editing based on QLineEdit but with special mouse control.

             DoubleEdit inherits QLineEdit and is restricted to double input.
	     The value can also be changed by the mouse wheel where each tic
	     increases or decreases the value by step.
*/
// --------------------------------------------------------------------------------

#ifndef DOUBLEEDIT_UTIL_H
#define DOUBLEEDIT_UTIL_H

#include <iostream>

#include <QLineEdit>

#include <gdefs.h>

// ---------------------------------------------------
//    class definition:   DoubleEdit
// ---------------------------------------------------
class DoubleEdit : public QLineEdit {
    Q_OBJECT

public:
    DoubleEdit(int prec, double value = 0.0, double step = 0.01, QWidget* parent = 0);
    ~DoubleEdit();

// --------- public methods -----------
public:
    void    setValue(double value);
    double  getValue();

    void    setStep(double step);
    double  getStep();

    void    setValueAndStep(double value, double step);
    void    getValueAndStep(double &value, double &step);

    void    setRange(double min, double max);
    void    getRange(double &min, double &max);

// --------- public slots -----------
public slots:
    void    slot_setValue();
    void    slot_setStep();


// --------- protected methods -----------
protected:
    virtual void wheelEvent(QWheelEvent* event);

// ------ protected attributes --------
protected:
    int    mPrecision;
    double mStep;
    double mMin;
    double mMax;
};

#endif
