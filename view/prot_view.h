/**
 * @file    prot_view.h
 * @author  Thomas Mueller
 *
 * @brief  Dialog for the protocol output.
 *
 * This file is part of GeodesicView.
 */
#ifndef PROTOCOL_VIEW_H
#define PROTOCOL_VIEW_H

#include <iostream>

#include <QAction>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include <gdefs.h>

#include <view/ledpub_widget.h>

/**
 * @brief The ProtDialog class
 */
class ProtDialog : public QWidget {
    Q_OBJECT

public:
    ProtDialog(QWidget* parent = nullptr);
    ~ProtDialog();

public:
    QString getDirName();
    QString getBaseFilename();

    bool doWrite3dImage();
    bool doWrite2dImage();
    bool doWriteGeodesic();
    bool doWriteViewParams();
    bool doWriteParameters();
    bool doWriteReport();
    bool doWriteObjects();

public slots:
    void slot_takeDir();
    void slot_write();
    void slot_close();

signals:
    void emitWriteProt();

protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();

private:
    QLabel* lab_dir;
    LedPub* lep_dir;
    QLabel* lab_filename;
    LedPub* lep_filename;

    QCheckBox* chb_save_3dimage;
    QCheckBox* chb_save_2dimage;
    QCheckBox* chb_save_geodesic;
    QCheckBox* chb_save_viewparams;
    QCheckBox* chb_save_parameters;
    QCheckBox* chb_save_report;
    QCheckBox* chb_save_objects;

    QPushButton* pub_write;
    QPushButton* pub_cancel;

    QString mDir;
};

#endif
