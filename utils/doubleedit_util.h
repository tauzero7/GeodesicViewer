/**
 * @file    doubleedit_util.h
 * @author  Thomas Mueller
 *
 * @brief  Line editing based on QLineEdit but with special mouse control.

    DoubleEdit inherits QLineEdit and is restricted to double input.
    The value can also be changed by the mouse wheel where each tic
        increases or decreases the value by step.

 * This file is part of GeodesicView.
 */
#ifndef DOUBLEEDIT_UTIL_H
#define DOUBLEEDIT_UTIL_H

#include <QLineEdit>
#include <gdefs.h>
#include <iostream>

/**
 * @brief The DoubleEdit class
 */
class DoubleEdit : public QLineEdit {
    Q_OBJECT

public:
    DoubleEdit(int prec, double value = 0.0, double step = 0.01, QWidget* parent = nullptr);
    ~DoubleEdit();

    // --------- public methods -----------
public:
    void setValue(double value);
    double getValue();

    void setStep(double step);
    double getStep();

    void setValueAndStep(double value, double step);
    void getValueAndStep(double& value, double& step);

    void setRange(double min, double max);
    void getRange(double& min, double& max);

    // --------- public slots -----------
public slots:
    void slot_setValue();
    void slot_setStep();

    // --------- protected methods -----------
protected:
    virtual void wheelEvent(QWheelEvent* event);

    // ------ protected attributes --------
protected:
    int mPrecision;
    double mStep;
    double mMin;
    double mMax;
};

#endif // DOUBLEEDIT_UTIL_H
