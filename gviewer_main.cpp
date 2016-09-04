// --------------------------------------------------------------------------------
/*
     gviewer_main.cpp     is part of the GeodesicViewer.

   Copyright (c) 2009-2015  Thomas Mueller, Frank Grave

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

 */

/*! \file   gviewer_main.cpp
    \brief  Main program of the GeodesicViewer

   \mainpage  GeodesicViewer

    <H2>Introduction</H2>
     The GeodesicViewer realizes exocentric two- and three-dimensional illustrations of lightlike and timelike geodesics in the general theory of relativity. By means of an intuitive graphical user interface, all parameters of a spacetime as well as the initial conditions of the geodesics can be modified interactively. This makes the GeodesicViewer a useful instrument for the exploration of geodesics in four-dimensional Lorentzian spacetimes.

    <H2>License</H2>
    <p>Copyright &copy; 2009-2011 by Thomas M&uuml;ller and Frank Grave</p><br>
    <p>Permission to use, copy, modify, and distribute this software and its documentation under the terms of the GNU General Public License is hereby granted. No representations are made about the suitability of this software for any purpose. It is provided "as is" without express or implied warranty. See the <a href="http://www.gnu.org/copyleft/gpl.html">GNU General Public License</a> for more details. </p>

    <br>
    <H2>Contact</H2>
    Visualisierungsinstitut der Universit&auml;t Stuttgart (VISUS)<br>
    Allmandring 19<br>
    70569 Stuttgart, Germany<br>
    Email: Thomas.Mueller@vis.uni-stuttgart.de<br><br>

*/
// --------------------------------------------------------------------------------

#include <iostream>

#include <QApplication>
#include <locale>

#include <geodesic_view.h>
#include <utils/myobject.h>
#include <utils/utilities.h>

#include <extra/m4dObject.h>
#if QT_VERSION < 0x050000
#include <QCleanlooksStyle>
#else
#include <QStyleFactory>
#endif


m4d::Object mObject;

// ---------------------------------------------------
//    m a i n
// ---------------------------------------------------
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QLocale::setDefault(QLocale::C);
    //QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    // setlocale(LC_NUMERIC, "C");

#if QT_VERSION >= 0x050100
    QApplication::setStyle(QStyleFactory::create("Fusion"));
#else
    QStyle *style = new QCleanlooksStyle;
    app.setStyle(style);
#endif

    QPalette  palette = app.palette();
    palette.setColor(QPalette::AlternateBase, QColor(217, 255, 190));
    app.setPalette(palette);

#if 0
    QFont font = app.font();
    font.setPointSize(16);
    app.setFont(font);
#endif

    GeodesicView* view = GeodesicView::getInstance();
    view->show();

    app.exec();
    return 0;
}
