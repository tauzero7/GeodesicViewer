// --------------------------------------------------------------------------------
/*
    ledpub_widget.h

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

/*!  \class  LedPub
     \brief  Dialog for the protocoll output.

*/
// --------------------------------------------------------------------------------

#ifndef LINEEDIT_PUSHBUTTON_WIDGET_H
#define LINEEDIT_PUSHBUTTON_WIDGET_H

#include <iostream>
#include <QtGui>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include <gdefs.h>

// ---------------------------------------------------
//    class definition:   qtLedPub
// ---------------------------------------------------
class LedPub : public QWidget {
    Q_OBJECT

public:
    LedPub(bool loadDir = false, QWidget* parent = 0);
    ~LedPub();

// --------- public methods -----------
public:
    void     setName(QString name);
    QString  getName();

    void     setDirPath(QString path);
    QString  getDirPath();

    void     setEnabled(bool enable);

// --------- public slots -------------
public slots:
    void  slot_load_file();


signals:
    void  dirAccepted();

// ------- protected methods ----------
protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();

// ------ protected attributes --------
protected:
    QLineEdit*    led_text;
    QPushButton*  pub_file;

    QString       mDirPath;
    bool          mLoadDir;
};

#endif

