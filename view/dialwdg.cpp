// --------------------------------------------------------------------------------
/*
    dialwdg.cpp

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
// -------------------------------------------------------------------------------

#include "dialwdg.h"

#ifndef RAD_TO_DEG
#define RAD_TO_DEG  57.295779513082320875
#define DEG_TO_RAD  0.017453292519943295770
#endif


/*! Constructor of the DialWidget
 * \param initValue : initial value.
 * \param step : step size
 * \param parent : parent widget.
 */
DialWidget::DialWidget(double initValue, double step, QWidget* parent)
    : QWidget(parent) {
    mInitValue = initValue;
    if (step == 0.0) {
        mStep = 0.01;
    } else {
        mStep = step;
    }
    arcMax  = 360.0;
}

/*!
 */
DialWidget::~DialWidget() {
}


// *********************************** public methods *********************************

/*!
 */
void DialWidget::reset() {
    mValue = mInitValue;
    repaint();
}

/*! Set compass range.
 * \param valMax : maximum value
 */
void DialWidget::setMax(double valMax) {
    arcMax = valMax;
}

/*! Set value.
 * \param val : value
 */
void DialWidget::setValue(double val) {
    mValue = val;
    if (mValue < 0.0) {
        mValue += arcMax;
    }

    mValue = fmod(mValue, arcMax);
    repaint();
}


/*! Set step.
 * \param step : step value
 */
void DialWidget::setStep(double step) {
    if (step != 0.0) {
        mStep = step;
    }
}

/*! Get value.
 * \return value
 */
double
DialWidget::value() {
    return mValue;
}

void DialWidget::doStep(int step) {
    setValue(mValue + step * mStep);
}

void DialWidget::setInitialValue(double value) {
    mInitValue = value;
}

// *********************************** protected methods *********************************

/*! Show event
 *  \param event : show event.
 */
void DialWidget::showEvent(QShowEvent * event) {
    dspWidth  = width();
    dspHeight = height();
    dspRadius = ((dspWidth > dspHeight) ? dspHeight : dspWidth) / 2 - 2;
    dspCenter = QPoint(dspWidth / 2 - 2, dspHeight / 2);

    event->accept();
}

/*! Paint event
 */
void DialWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QBrush brsh(QColor(0, 0, 0));
    painter.setBrush(brsh);

    double ptrRadius = dspRadius * 0.6;
    painter.drawEllipse(dspCenter, int(dspRadius * 0.99), int(dspRadius * 0.99));

    brsh.setColor(QColor(70, 70, 70));
    painter.setBrush(brsh);
    painter.drawEllipse(dspCenter, int(ptrRadius * 1.1), int(ptrRadius * 1.1));

    QPen ptr(QColor(255, 255, 255));
    ptr.setWidth(3);
    painter.setPen(ptr);

    painter.drawLine(dspCenter, dspCenter + QPointF(ptrRadius * sin(mValue * DEG_TO_RAD), -ptrRadius * cos(mValue * DEG_TO_RAD)));

    ptr.setColor(QColor(210, 210, 200));
    ptr.setWidth(2);
    painter.setPen(ptr);
    int pr = int(ptrRadius * 1.09);
    painter.drawArc(dspCenter.x() - pr, dspCenter.y() - pr, 2 * pr, 2 * pr, 90 * 16, -int(arcMax * 16));

    for (int i = 0; i <= arcMax; i += 10) {
        double fk = 1.2;
        if (i % 30 != 0) {
            fk = 1.15;
        }
        double sx = sin(i * DEG_TO_RAD);
        double cx = cos(i * DEG_TO_RAD);
        QPoint p1 = dspCenter + QPoint(ptrRadius * sx * fk, ptrRadius * cx * fk);
        QPoint p2 = dspCenter + QPoint(ptrRadius * sx * 1.1, ptrRadius * cx * 1.1);
        painter.drawLine(p1, p2);
    }
    QFont Font = QFont("Helvetica", 9);
    Font.setBold(true);
    painter.setFont(Font);
    for (int i = 0; i <= arcMax; i += 60) {
        double sx = sin(i * DEG_TO_RAD);
        double cx = cos(i * DEG_TO_RAD);
        QPoint pf = dspCenter + QPoint(1.45 * ptrRadius * sx, -1.45 * ptrRadius * cx);
        QRectF rect(pf.x() - 15.0, pf.y() - 9, 30, 18);
        if (i < 360) {
            painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(i));
        }
    }
}

/*! Mouse move event
 *  \param event : mouse event.
 */
void DialWidget::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = event->pos() - dspCenter;
    posToAngle(pos);
    repaint();
    emit dialValueChanged(mValue);
}

/*! Mouse press event
 *  \param event : mouse event.
 */
void DialWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->pos() - dspCenter;
        posToAngle(pos);
        emit dialValueChanged(mValue);
    }
}

/*! Wheel event
 */
void DialWidget::wheelEvent(QWheelEvent *event) {
    int delta = event->delta();
    doStep((delta > 0 ? 1 : -1));
    repaint();
    emit dialValueChanged(mValue);
}

/*! Transform pointer position to angle.
 * \param pos : pointer position.
 */
void DialWidget::posToAngle(QPoint pos) {
    mValue = angle = floor((atan2((double) - pos.x(), (double)pos.y()) + M_PI) * RAD_TO_DEG / mStep) * mStep;
    if (angle < 0.0) {
        mValue = 0.0;
    }
    if (angle > arcMax) {
        mValue = arcMax;
    }
    setValue(mValue);
}
