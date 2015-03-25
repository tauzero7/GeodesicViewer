// --------------------------------------------------------------------------------
/*
    report_view.cpp

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
// -------------------------------------------------------------------------------

#include <QGridLayout>
#include "report_view.h"

/*! Standard constructor.
 * \param  parent: pointer to parent widget.
 */
ReportDialog::ReportDialog(QWidget* parent)
    : QDialog(parent) {
    init();
}

/*! Standard destructor.
 */
ReportDialog::~ReportDialog() {
}


// *********************************** public methods *********************************
/*!
 */
void ReportDialog::setText(std::string text) {
    ted_report->clear();
    ted_report->setText(text.c_str());
}

// ************************************* public slots ***********************************
/*!
 */
void ReportDialog::slot_close() {
    hide();
}


// *********************************** protected methods *********************************

/*! Initialize view.
 */
void ReportDialog::init() {
    initElements();
    initGUI();
    initControl();

    setModal(false);

}

/*! Initialize view elements.
 */
void ReportDialog::initElements() {
    pub_close = new QPushButton("Close");

    ted_report = new QTextEdit();
    ted_report->setReadOnly(true);

    QFont mfont("Courier", 10);
    ted_report->setFont(mfont);
}

/*! Construct graphical user interface.
 */
void ReportDialog::initGUI() {
    QGridLayout* layout_complete = new QGridLayout();
    layout_complete->addWidget(ted_report,  0, 0);
    layout_complete->addWidget(pub_close,  1, 0);
    setLayout(layout_complete);

    setGeometry(0, 0, 700, 300);
    setWindowTitle("GeodesicViewer - Report");
}


/*! Connect signals and slots.
 */
void ReportDialog::initControl() {
    connect(pub_close, SIGNAL(pressed()), this, SLOT(slot_close()));
}
