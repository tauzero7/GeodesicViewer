/**
 * @file    gviewer_main.h
 * @author  Thomas Mueller
 *
 * @brief  Main program of the GeodesicViewer
 *
 *   \mainpage  GeodesicViewer

    <H2>Introduction</H2>
     The GeodesicViewer realizes exocentric two- and three-dimensional illustrations of lightlike and timelike geodesics in the general theory of relativity. By means of an intuitive graphical user interface, all parameters of a spacetime as well as the initial conditions of the geodesics can be modified interactively. This makes the GeodesicViewer a useful instrument for the exploration of geodesics in four-dimensional Lorentzian spacetimes.

    <H2>License</H2>
    <p>Copyright &copy; 2009-2011 by Thomas M&uuml;ller and Frank Grave</p><br>
    <p>Permission to use, copy, modify, and distribute this software and its documentation under the terms of the GNU General Public License is hereby granted. No representations are made about the suitability of this software for any purpose. It is provided "as is" without express or implied warranty. See the <a href="http://www.gnu.org/copyleft/gpl.html">GNU General Public License</a> for more details. </p>

    <br>
    <H2>Contact</H2>
    Haus der Astronomie<br>
    Königstuhl 17<br>
    69117 Heidelberg, Germany<br>
    Email: tmueller@mpia.de<br><br>
 *
 * This file is part of GeodesicView.
 */
#include <iostream>

#include <QApplication>
#include <locale>

#include <geodesic_view.h>
#include <utils/myobject.h>
#include <utils/utilities.h>
#include <view/mapplication.h>
#include <extra/m4dObject.h>

m4d::Object mObject;

// ---------------------------------------------------
//    m a i n
// ---------------------------------------------------
int main(int argc, char *argv[]) {    
    QLocale::setDefault(QLocale::C);

    char* exePath = nullptr;
    GetExePath(exePath);

    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    MApplication app(argc, argv, exePath);
    if (exePath != nullptr) {
        delete [] exePath;
    }

    GeodesicView* view = GeodesicView::getInstance();
    view->show();

    return app.exec();
}
