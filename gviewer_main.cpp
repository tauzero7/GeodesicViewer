/**
 * @file    gviewer_main.h
 * @author  Thomas Mueller
 *
 * @brief  Main program of the GeodesicViewer
 *
 *   \mainpage  GeodesicViewer

    <H2>Introduction</H2>
     The GeodesicViewer realizes exocentric two- and three-dimensional illustrations of lightlike and timelike
     geodesics in the general theory of relativity. By means of an intuitive graphical user interface, all parameters
     of a spacetime as well as the initial conditions of the geodesics can be modified interactively. This makes the
     GeodesicViewer a useful instrument for the exploration of geodesics in four-dimensional Lorentzian spacetimes.

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

#include <extra/m4dObject.h>
#include <geodesic_view.h>
#include <utils/myobject.h>
#include <utils/utilities.h>
#include <view/mapplication.h>

m4d::Object mObject;

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
    QLocale::setDefault(QLocale::C);

    char* exePath = nullptr;
    GetExePath(exePath);

    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    MApplication app(argc, argv, exePath);
    if (exePath != nullptr) {
        delete[] exePath;
    }

    GeodesicView* view = GeodesicView::getInstance();
    view->show();

    return app.exec();
}
