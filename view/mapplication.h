/**
 * @file    mapplication.h
 * @author  Thomas Mueller
 *
 * @brief  QApplication for every Qt frontend loading css file and dark palette.
 *
 * This file is part of GeodesicView.
 */
#ifndef MAPPLICATION_H
#define MAPPLICATION_H

#include <QApplication>
#include <QCoreApplication>
#include <QStyleFactory>

#include "dark_palette.h"

class MApplication : public QApplication {
public:
    MApplication(int &argc, char **argv, char* exePath);
};

#endif // MAPPLICATION_H
