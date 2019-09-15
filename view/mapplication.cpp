/**
 * @file    mapplication.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "mapplication.h"
#include <QFile>
#include <QTextStream>

MApplication::MApplication(int& argc, char** argv, char* exePath)
    : QApplication(argc, argv)
{
    setStyle(QStyleFactory::create("Fusion"));

    DarkPalette darkPalette;
    setPalette(darkPalette);

#ifdef _WIN32
    QString cssFilename = QString(exePath) + "/viewer_win.css";
#else
    QString cssFilename = QString(exePath) + "/viewer.css";
#endif
    QString cssStyle;
    QFile cssFile(cssFilename);
    if (cssFile.open(QFile::ReadOnly)) {
        QTextStream styleIn(&cssFile);
        while (!styleIn.atEnd()) {
            QString line = styleIn.readLine();
            if (!line.startsWith('#')) {
                cssStyle += line;
            }
        }
        cssFile.close();
        setStyleSheet(cssStyle);
    } else {
        fprintf(stderr, "Could not open stylesheet file '%s'.\n", cssFilename.toStdString().c_str());
    }
}
