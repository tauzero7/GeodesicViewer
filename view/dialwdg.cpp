/**
 * @file    dialwdg.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "dialwdg.h"
#include "utils/mathutils.h"

DialWidget::DialWidget(double initValue, double step, QWidget* parent)
    : QWidget(parent)
{
    mInitValue = initValue;
    if (step == 0.0) {
        mStep = 0.01;
    }
    else {
        mStep = step;
    }
    arcMax = 360.0;
}

DialWidget::~DialWidget() {}

void DialWidget::reset()
{
    mValue = mInitValue;
    repaint();
}

void DialWidget::setMax(double valMax)
{
    arcMax = valMax;
}

void DialWidget::setValue(double val)
{
    mValue = val;
    if (mValue < 0.0) {
        mValue += arcMax;
    }

    mValue = fmod(mValue, arcMax);
    repaint();
}

void DialWidget::setStep(double step)
{
    if (step != 0.0) {
        mStep = step;
    }
}

double DialWidget::value()
{
    return mValue;
}

void DialWidget::doStep(int step)
{
    setValue(mValue + step * mStep);
}

void DialWidget::setInitialValue(double value)
{
    mInitValue = value;
}

void DialWidget::showEvent(QShowEvent* event)
{
    dspWidth = width();
    dspHeight = height();
    dspRadius = ((dspWidth > dspHeight) ? dspHeight : dspWidth) / 2 - 2;
    dspCenter = QPoint(dspWidth / 2 - 2, dspHeight / 2);

    event->accept();
}

void DialWidget::paintEvent(QPaintEvent*)
{
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

    painter.drawLine(
        dspCenter, dspCenter + QPointF(ptrRadius * sin(mValue * DEG_TO_RAD), -ptrRadius * cos(mValue * DEG_TO_RAD)));

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
        QPointF p1 = dspCenter + QPointF(ptrRadius * sx * fk, ptrRadius * cx * fk);
        QPointF p2 = dspCenter + QPointF(ptrRadius * sx * 1.1, ptrRadius * cx * 1.1);
        painter.drawLine(p1, p2);
    }

    QFont Font = QFont("Helvetica", 9);
    Font.setBold(true);
    painter.setFont(Font);
    for (int i = 0; i <= arcMax; i += 60) {
        double sx = sin(i * DEG_TO_RAD);
        double cx = cos(i * DEG_TO_RAD);
        QPointF pf = dspCenter + QPointF(1.45 * ptrRadius * sx, -1.45 * ptrRadius * cx);
        QRectF rect(pf.x() - 15.0, pf.y() - 9, 30, 18);
        if (i < 360) {
            painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(i));
        }
    }
}

void DialWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos() - dspCenter;
    posToAngle(pos);
    repaint();
    emit dialValueChanged(mValue);
    event->accept();
}

void DialWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->pos() - dspCenter;
        posToAngle(pos);
        emit dialValueChanged(mValue);
        event->accept();
    }
    event->ignore();
}

void DialWidget::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta();
    doStep((delta > 0 ? 1 : -1));
    repaint();
    emit dialValueChanged(mValue);
    event->accept();
}

void DialWidget::posToAngle(QPoint pos)
{
    double px = static_cast<double>(pos.x());
    double py = static_cast<double>(pos.y());

    mValue = angle = floor((atan2(-px, py) + PI) * RAD_TO_DEG / mStep) * mStep;
    mValue = std::max(std::min(mValue, arcMax), 0.0);
    setValue(mValue);
}
