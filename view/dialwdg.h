/**
 * @file    dialwdg.h
 * @author  Thomas Mueller
 *
 * @brief  Dial Widget
 *
 * This file is part of GeodesicView.
 */
#ifndef DIAL_WIDGET_H
#define DIAL_WIDGET_H

#include <cmath>
#include <iostream>

#include <QMouseEvent>
#include <QPainter>
#include <QWidget>

#define DEF_COMPASS_DIAL_STEP 1.0
#define DEF_COMPASS_DIAL_PRECISION 7

/**
 * @brief The DialWidget class
 */
class DialWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief DialWidget c'tor.
     * @param initValue  Initial value.
     * @param step  Step size.
     * @param parent  Parent widget.
     */
    DialWidget(double initValue, double step, QWidget* parent = nullptr);
    ~DialWidget();

public:
    void reset();
    void setMax(double valMax);
    void setValue(double val);
    void setStep(double step);
    double value();
    void doStep(int step);

    void setInitialValue(double value);

signals:
    void dialValueChanged(double);

protected:
    void showEvent(QShowEvent* event);
    void paintEvent(QPaintEvent* event);

    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    /**
     * @brief Transform pointer position to angle.
     * @param pos
     */
    void posToAngle(QPoint pos);

private:
    QPoint dspCenter;
    int dspRadius;
    int dspWidth;
    int dspHeight;

    double arcMax;
    double angle;

    double mInitValue;
    double mValue;
    double mStep;
};

#endif
