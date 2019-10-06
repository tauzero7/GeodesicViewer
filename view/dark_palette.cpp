/**
 * @file    dark_palette.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */

#include "dark_palette.h"

DarkPalette::DarkPalette()
{
    setColor(QPalette::Window, QColor(53, 53, 53));
    setColor(QPalette::WindowText, Qt::white);
    setColor(QPalette::Base, QColor(25, 25, 25));
    setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    setColor(QPalette::ToolTipBase, Qt::gray);
    setColor(QPalette::ToolTipText, Qt::white);
    setColor(QPalette::Text, Qt::white);
    setColor(QPalette::Button, QColor(53, 53, 53));
    setColor(QPalette::ButtonText, Qt::white);
    setColor(QPalette::BrightText, Qt::red);
    setColor(QPalette::Link, QColor(120, 120, 120));
    setColor(QPalette::Highlight, QColor(0xc6, 0xc6, 0xff));
    setColor(QPalette::HighlightedText, Qt::black);
}
