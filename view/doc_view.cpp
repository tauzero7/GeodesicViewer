// --------------------------------------------------------------------------------
/*
    geod_view.cpp

  Copyright (c) 2009-2013  Thomas Mueller, Frank Grave


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

#include <QCoreApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include "doc_view.h"

DocView::DocView() {
    init();
}

// *********************************** protected methods *********************************

/*! Initialize view.
 */
void DocView::init() {
    initElements();
    initGUI();
    initControl();
}

/*! Initialize view elements.
 */
void DocView::initElements() {
    view = new QWebView();
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
#ifdef _WIN32
    QString manpage = QCoreApplication::applicationDirPath() + QString("/../doc/manual/index.html");
#else
    QString manpage = QCoreApplication::applicationDirPath() + QString("/doc/manual/index.html");
#endif
    view->setUrl(QUrl(manpage));

    pub_back    = new QPushButton(QIcon(":/back.png"), "back");
    pub_forward = new QPushButton(QIcon(":/forward.png"), "forward");

    pub_close = new QPushButton(QIcon(":/exit.png"), "close");
}

/*! Construct graphical user interface.
 */
void DocView::initGUI() {
    QGridLayout* layout_complete = new QGridLayout();
    layout_complete -> addWidget(view, 0, 0);

    QHBoxLayout* layout_bottom = new QHBoxLayout();
    layout_bottom -> addWidget(pub_back);
    layout_bottom -> addWidget(pub_forward);
    layout_bottom -> addWidget(pub_close);

    layout_complete -> addLayout(layout_bottom, 1, 0);
    setLayout(layout_complete);
    setWindowTitle("GeodesicViewer - Manual");
}

/*! Connect signals and slots.
 */
void DocView::initControl() {
    connect(pub_back,    SIGNAL(pressed()), view, SLOT(back()));
    connect(pub_forward, SIGNAL(pressed()), view, SLOT(forward()));
    connect(pub_close,   SIGNAL(pressed()), this, SLOT(hide()));
}
