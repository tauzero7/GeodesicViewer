/**
 * @file    doubleedit_util.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "doubleedit_util.h"
#include <QWheelEvent>

DoubleEdit::DoubleEdit(int prec, double value, double step, QWidget* parent)
    : QLineEdit(parent)
{
    mPrecision = prec;
    mStep = step;
    setValue(value);
    setAlignment(Qt::AlignRight);

    mMin = -DBL_MAX;
    mMax = DBL_MAX;

    setPalette(QPalette(QColor(DEF_DOUBLE_EDIT_COLOR)));
}

DoubleEdit::~DoubleEdit()
{
}

void DoubleEdit::setValue(double value)
{
    setText(QString::number(value, 'f', mPrecision));
}

double
DoubleEdit::getValue()
{
    /*
    double val = atof(text().toStdString().c_str());
    fprintf(stderr,"%15.12f  %15.12f\n",val,text().toDouble());
    */
    return text().toDouble();
}

void DoubleEdit::setStep(double step)
{
    mStep = step;
}

double
DoubleEdit::getStep()
{
    return mStep;
}

void DoubleEdit::setValueAndStep(double value, double step)
{
    setValue(value);
    setStep(step);
}

void DoubleEdit::getValueAndStep(double& value, double& step)
{
    value = getValue();
    step = getStep();
}

void DoubleEdit::setRange(double min, double max)
{
    mMin = min;
    mMax = max;
}

void DoubleEdit::getRange(double& min, double& max)
{
    min = mMin;
    max = mMax;
}

void DoubleEdit::slot_setValue()
{
    QObject* obj = sender();
    DoubleEdit* led = reinterpret_cast<DoubleEdit*>(obj);
    double value = led->getValue();
    setValue(value);
}

void DoubleEdit::slot_setStep()
{
    QObject* obj = sender();
    DoubleEdit* led = reinterpret_cast<DoubleEdit*>(obj);
    double value = led->getValue();
    setStep(value);
}

void DoubleEdit::wheelEvent(QWheelEvent* event)
{
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
