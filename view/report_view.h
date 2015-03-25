// --------------------------------------------------------------------------------
/*
    report_view.h

  Copyright (c) 2010  Thomas Mueller


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

/*!  \class  ReportDialog
     \brief  Dialog for the report output.

*/
// --------------------------------------------------------------------------------

#ifndef REPORT_VIEW_H
#define REPORT_VIEW_H

#include <iostream>

#include <QDialog>
#include <QPushButton>
#include <QTextEdit>

#include <gdefs.h>


// ---------------------------------------------------
//    class definition:   ReportDialog
// ---------------------------------------------------
class ReportDialog : public QDialog {
    Q_OBJECT

public:
    ReportDialog(QWidget* parent = 0);
    ~ReportDialog();

// ------------ public methods -------------
public:
    void  setText(std::string text);

// ------------ public slots -------------
public slots:
    void  slot_close();



// ----------- protected methods -----------
protected:
    void  init();
    void  initElements();
    void  initGUI();
    void  initControl();

// ----------- private attributes ----------
private:

    QPushButton*  pub_close;

    QTextEdit*    ted_report;
};

#endif

