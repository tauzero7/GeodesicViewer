// --------------------------------------------------------------------------------
/*
    dialwdg.h

  Copyright (c) 2010  Thomas Mueller


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

/*!  \class  DialWidget
     \brief  DialWidget

*/
// --------------------------------------------------------------------------------

#ifndef DIAL_WIDGET_H
#define DIAL_WIDGET_H

#include <iostream>
#include <cmath>

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

#define  DEF_COMPASS_DIAL_STEP       1.0
#define  DEF_COMPASS_DIAL_PRECISION  7

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

// ---------------------------------------------------
//    class definition:   DialWidget
// ---------------------------------------------------
class DialWidget : public QWidget {
    Q_OBJECT

public:
    DialWidget(double initValue, double step, QWidget* parent = 0);
    ~DialWidget();

// ------------ public methods -------------
public:
    void    reset();
    void    setMax(double valMax);
    void    setValue(double val);
    void    setStep(double step);
    double  value();
    void    doStep(int step);

    void    setInitialValue(double value);

signals:
    void   dialValueChanged(double);

// ----------- protected methods -----------
protected:
    void  showEvent(QShowEvent * event);
    void  paintEvent(QPaintEvent *event);

    void  mouseMoveEvent(QMouseEvent *event);
    void  mousePressEvent(QMouseEvent * event);
    void  wheelEvent(QWheelEvent *event);

    void  posToAngle(QPoint pos);

// ----------- private attributes ----------
private:
    QPoint   dspCenter;
    int      dspRadius;
    int      dspWidth;
    int      dspHeight;

    double   arcMax;
    double   angle;

    double   mInitValue;
    double   mValue;
    double   mStep;
};

#endif
