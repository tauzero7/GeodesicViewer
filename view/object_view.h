// --------------------------------------------------------------------------------
/*
    object_view.h

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

/*!  \class  ObjectView
     \brief  View for object manipulations.

*/
// --------------------------------------------------------------------------------

#ifndef OBJECT_VIEW_H
#define OBJECT_VIEW_H

#include <iostream>

#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScriptEngine>
#include <QTextEdit>
#include <QWidget>

#include <gdefs.h>
#include <utils/myobject.h>


// ---------------------------------------------------
//    class definition:   ObjectView
// ---------------------------------------------------
class ObjectView : public QWidget {
    Q_OBJECT

public:
    ObjectView(QWidget* parent = 0);
    ~ObjectView();

// ------------ public methods -------------
public:
    void    writeText(QString text);
    QString getText();

    void  addObjectsToScriptEngine(QScriptEngine* engine);

// ------------ public slots -------------
public slots:
    void  slot_showSyntax(int index);
    void  slot_apply();
    void  slot_close();
    void  slot_clear();
    void  slot_add_object();

    // script functions
    void  addObject(QString objectText);
    void  clearAll();

// ------------ signals -------------
signals:
    void  objectsChanged();

// ----------- protected methods -----------
protected:
    void  init();
    void  initElements();
    void  initGUI();
    void  initActions();
    void  initControl();

// ----------- private attributes ----------
private:
    QLabel*       lab_text;
    QTextEdit*    ted_objects;
    QPushButton*  pub_apply;
    QPushButton*  pub_close;
    QPushButton*  pub_clear;

    QAction*      mActionApply;

    QLabel*       lab_syntax;
    QLineEdit*    led_syntax;

    QLabel*       lab_add_object;
    QComboBox*    cob_objects;
    QPushButton*  pub_add_object;
};

#endif

