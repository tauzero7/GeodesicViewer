// --------------------------------------------------------------------------------
/*
    compass_dial.h

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

/*!  \class  CompassDial
     \brief  Combination of Qwt-Compass and DoubleEdit.

*/
// --------------------------------------------------------------------------------

#ifndef COMPASS_DIAL_H
#define COMPASS_DIAL_H

#include <iostream>

#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <dialwdg.h>
#include <doubleedit_util.h>
#include <greek.h>

#define DEF_COMPASS_DIAL_STEP 1.0
#define DEF_COMPASS_DIAL_PRECISION 7

// ---------------------------------------------------
//    class definition:   dcpDial
// ---------------------------------------------------
class CompassDial : public QWidget {
    Q_OBJECT

public:
    CompassDial(QString valName = "", QWidget* parent = nullptr);
    ~CompassDial();

    // ------------ public methods -------------
public:
    void reset();
    void setValue(double val);
    void setRange(double valMax);
    void setStep(double step);
    double value();
    void doStep(int step);

    void setInitValue(double val);

signals:
    void compassDialValueChanged(double);

    // ----------- protected methods -----------
protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();

    // ------------ public slots -------------
public slots:
    void set(double val);

    // ----------- protected slots -----------
protected slots:
    void slot_stepChanged();
    void slot_paramChanged();
    void slot_reset();

    // ----------- private attributes ----------
private:
    DialWidget* dial;
    QLabel* lab_param_val;
    DoubleEdit* led_param_val;
    QLabel* lab_param_step;
    DoubleEdit* led_param_step;
    QPushButton* pub_reset;
    QString mValueName;

    GreekLetter mGreekLetter;

    double mInitValue;
    double mValue;
    double mStep;
};

#endif
