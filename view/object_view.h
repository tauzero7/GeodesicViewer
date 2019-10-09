/**
 * @file    object_view.h
 * @author  Thomas Mueller
 *
 * @brief  View for object manipulations.
 *
 * This file is part of GeodesicView.
 */
#ifndef OBJECT_VIEW_H
#define OBJECT_VIEW_H

#include <iostream>

#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include <gdefs.h>
#include <utils/myobject.h>

/**
 * @brief The ObjectView class
 */
class ObjectView : public QWidget {
    Q_OBJECT

public:
    ObjectView(QWidget* parent = nullptr);
    ~ObjectView();

public:
    void writeText(QString text);
    QString getText();

public slots:
    void slot_showSyntax(int index);
    void slot_apply();
    void slot_close();
    void slot_clear();
    void slot_add_object();

    // script functions
    void addObject(QString objectText);
    void clearAll();

signals:
    void objectsChanged();

protected:
    void init();
    void initElements();
    void initGUI();
    void initActions();
    void initControl();

private:
    QLabel* lab_text;
    QTextEdit* ted_objects;
    QPushButton* pub_apply;
    QPushButton* pub_close;
    QPushButton* pub_clear;

    QAction* mActionApply;

    QLabel* lab_syntax;
    QLineEdit* led_syntax;

    QLabel* lab_add_object;
    QComboBox* cob_objects;
    QPushButton* pub_add_object;
};

#endif
