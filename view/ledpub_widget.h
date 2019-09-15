/**
 * @file    ledpub_widget.h
 * @author  Thomas Mueller
 *
 * @brief  Dialog for the protocoll output.
 *
 * This file is part of GeodesicView.
 */
#ifndef LINEEDIT_PUSHBUTTON_WIDGET_H
#define LINEEDIT_PUSHBUTTON_WIDGET_H

#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QtGui>
#include <iostream>

#include <gdefs.h>

/**
 * @brief The LedPub class
 */
class LedPub : public QWidget {
    Q_OBJECT

public:
    LedPub(bool loadDir = false, QWidget* parent = nullptr);
    ~LedPub();

public:
    void setName(QString name);
    QString getName();

    void setDirPath(QString path);
    QString getDirPath();

    void setEnabled(bool enable);

public slots:
    void slot_load_file();

signals:
    void dirAccepted();

protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();

protected:
    QLineEdit* led_text;
    QPushButton* pub_file;

    QString mDirPath;
    bool mLoadDir;
};

#endif
