// --------------------------------------------------------------------------------
/*
    prot_view.h

  Copyright (c) 2009-2015  Thomas Mueller, Frank Grave


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

/*!  \class  ProtDialog
     \brief  Dialog for the protocol output.

*/
// --------------------------------------------------------------------------------

#ifndef PROTOCOL_VIEW_H
#define PROTOCOL_VIEW_H

#include <iostream>

#include <QAction>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QScriptEngine>
#include <QTextEdit>
#include <QWidget>

#include <gdefs.h>

#include <view/ledpub_widget.h>


// ---------------------------------------------------
//    class definition:   ProtDialog
// ---------------------------------------------------
class ProtDialog : public QWidget {
    Q_OBJECT

public:
    ProtDialog(QWidget* parent = 0);
    ~ProtDialog();

// ------------ public methods -------------
public:
    QString  getDirName();
    QString  getBaseFilename();

    bool     doWrite3dImage();
    bool     doWrite2dImage();
    bool     doWriteGeodesic();
    bool     doWriteViewParams();
    bool     doWriteParameters();
    bool     doWriteReport();
    bool     doWriteObjects();

// ------------ public slots -------------
public slots:
    void  slot_takeDir();
    void  slot_write();
    void  slot_close();


// ------------ signals -------------
signals:
    void  emitWriteProt();

// ----------- protected methods -----------
protected:
    void  init();
    void  initElements();
    void  initGUI();
    void  initControl();

// ----------- private attributes ----------
private:
    QLabel*       lab_dir;
    LedPub*       lep_dir;
    QLabel*       lab_filename;
    LedPub*       lep_filename;

    QCheckBox*    chb_save_3dimage;
    QCheckBox*    chb_save_2dimage;
    QCheckBox*    chb_save_geodesic;
    QCheckBox*    chb_save_viewparams;
    QCheckBox*    chb_save_parameters;
    QCheckBox*    chb_save_report;
    QCheckBox*    chb_save_objects;

    QPushButton*  pub_write;
    QPushButton*  pub_cancel;

    QString       mDir;
};

#endif

