/**
 * @file    compass_dial.h
 * @author  Thomas Mueller
 *
 * @brief  Combination of dial widget and doubleedit.
 *
 * This file is part of GeodesicView.
 */
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

/**
 * @brief The CompassDial class
 */
class CompassDial : public QWidget {
    Q_OBJECT

public:
    CompassDial(QString valName = "", QWidget* parent = nullptr);
    ~CompassDial();

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

protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();

public slots:
    void set(double val);

protected slots:
    void slot_stepChanged();
    void slot_paramChanged();
    void slot_reset();

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
