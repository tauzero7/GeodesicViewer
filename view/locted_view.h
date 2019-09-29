/**
 * @file    locted_view.h
 * @author  Thomas Mueller
 *
 * @brief  View for local tetrad manipulation.
 *
 * This file is part of GeodesicView.
 */
#ifndef LOCALTETRAD_VIEW_H
#define LOCALTETRAD_VIEW_H

#include <iostream>

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>

#include <doubleedit_util.h>
#include <gdefs.h>
#include <gramschmidt.h>
#include <greek.h>

#include <extra/m4dObject.h>
#include <m4dGlobalDefs.h>

/**
 * @brief The LoctedView class
 */
class LoctedView : public QGroupBox
{
    Q_OBJECT

public:
    LoctedView(QWidget* parent = nullptr);
    ~LoctedView();

public:
    void updateData();
    void resetAll();

    void adjustMetric();
    void adjustCoordNames();
    void adjustLocTedTypes();

    void setLocalTetrad(bool reset);

    void setBoostParams(double chi, double ksi, double vel);
    void resetBoost();

    // void  addObjectsToScriptEngine(QScriptEngine* engine);

public slots:
    void slot_setPosition();
    void slot_setPosStep();
    void slot_decrPosition();
    void slot_incrPosition();
    void slot_resetPosition();

    void slot_setLoctedType();
    void slot_resetLocalTetrad();
    void slot_setLocalTetrad();
    void slot_orthoLocalTetrad();
    void slot_setPredefTetrad(int);

    void slot_setBoostParams();
    void slot_setBoostParamSteps();
    void slot_resetBoost();

    // script functions
    void setPosition(double x0, double x1, double x2, double x3);
    void setPosition(int num, double val);
    void setPredefTetrad(int num);
    void setPredefTetrad(QString name);

signals:
    void calcGeodesic();

protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();
    void initStatusTips();

private:
    QLabel* lab_value;
    QLabel* lab_step;

    QTabWidget* tab_locted;
    QLabel* lab_locted_type;
    QComboBox* cob_locted_type;

    // ----------------------
    //     position
    // ----------------------
    QList<QLabel*> lst_lab_locted_coordname;
    QList<DoubleEdit*> lst_led_locted_pos_value;
    QList<DoubleEdit*> lst_led_locted_pos_step;
    QList<QPushButton*> lst_pub_locted_pos_incr;
    QList<QPushButton*> lst_pub_locted_pos_decr;
    QPushButton* pub_pos_reset;
    QWidget* wgt_locted_position;

    // ----------------------
    //    tetrad direction
    // ----------------------
    QTableWidget* tbw_locted_tetrad;
    QPushButton* pub_locted_reset;
    QPushButton* pub_locted_set;
    QPushButton* pub_locted_ortho;
    QComboBox* cob_locted_predef;
    QWidget* wgt_locted_tetrads;

    // ----------------------
    //     tetrad boost
    // ----------------------
    QWidget* wgt_locted_boost;
    QLabel* lab_boost_value;
    QLabel* lab_boost_step;
    QLabel* lab_boost_chi;
    DoubleEdit* led_boost_chi_value;
    DoubleEdit* led_boost_chi_step;
    QLabel* lab_boost_ksi;
    DoubleEdit* led_boost_ksi_value;
    DoubleEdit* led_boost_ksi_step;
    QLabel* lab_boost_vel;
    DoubleEdit* led_boost_vel_value;
    DoubleEdit* led_boost_vel_step;
    QPushButton* pub_boost_reset;

    GreekLetter mGreekLetter;
    GramSchmidt* mGramSchmidt;
};

#endif
