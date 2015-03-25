// --------------------------------------------------------------------------------
/*
    ledpub_widget.cpp

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
// -------------------------------------------------------------------------------

#include <QHBoxLayout>
#include "ledpub_widget.h"

// ---------------------------------------------------
//    constructur/destructor
// ---------------------------------------------------
LedPub::LedPub(bool loadDir, QWidget* parent)
    : QWidget(parent) {
    mLoadDir = loadDir;
    mDirPath = QString();
    init();
}

LedPub::~LedPub() {
}

// ********************************** public methods *****************************
/*! Set name.
 * \param filename : name of file.
 */
void LedPub::setName(QString filename) {
    led_text->setText(filename);
}

/*! Get name.
 * \return name of file.
 */
QString LedPub::getName() {
    return led_text->text();
}

/*! Set directory path.
 * \param path : name of path.
 */
void LedPub::setDirPath(QString path) {
    mDirPath = path;
}

/*! Get directory path.
 * \return name of path.
 */
QString LedPub::getDirPath() {
    return mDirPath;
}

/*!
 */
void LedPub::setEnabled(bool enable) {
    led_text->setEnabled(enable);
    pub_file->setEnabled(enable);
}


// *********************************** public slots ******************************
/*! Load file
 */
void LedPub::slot_load_file() {
    QString name, path;

    if (mLoadDir) {
        QDir cdir = QDir::current();
        cdir.cd(DEF_STANDARD_DATA_DIRECTORY);
        name = QFileDialog::getExistingDirectory(this, tr("Choose directory"), cdir.absolutePath(), QFileDialog::ShowDirsOnly);
    } else {
        path = mDirPath;
        if (path == QString()) {
            path = QDir::currentPath();
        }
        name = QFileDialog::getSaveFileName(this, tr("Choose file"), path, tr("All files (*.*)"));
    }

    if (name != QString()) {
        led_text->setText(name);
        if (mLoadDir) {
            emit dirAccepted();
        }
    }
}

// ********************************* protected methods *****************************

/*! Initialize view.
 */
void LedPub::init() {
    initElements();
    initGUI();
    initControl();
}

/*! Initialize view elements.
 */
void LedPub::initElements() {
    led_text = new QLineEdit();
    led_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pub_file = new QPushButton("...");
    pub_file->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
    pub_file->setMaximumWidth(30);
}

/*! Construct graphical user interface.
 */
void LedPub::initGUI() {
    QHBoxLayout* layout = new QHBoxLayout();

    layout->addWidget(led_text);
    layout->addWidget(pub_file);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
}

/*! Connect signals and slots.
 */
void LedPub::initControl() {
    connect(pub_file, SIGNAL(pressed()), this, SLOT(slot_load_file()));
}
