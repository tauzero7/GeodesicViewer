/**
 * @file    ledpub_widget.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "ledpub_widget.h"
#include <QHBoxLayout>

LedPub::LedPub(bool loadDir, QWidget* parent)
    : QWidget(parent)
{
    mLoadDir = loadDir;
    mDirPath = QString();
    init();
}

LedPub::~LedPub()
{
}

void LedPub::setName(QString filename)
{
    led_text->setText(filename);
}

QString LedPub::getName()
{
    return led_text->text();
}

void LedPub::setDirPath(QString path)
{
    mDirPath = path;
}

QString LedPub::getDirPath()
{
    return mDirPath;
}

void LedPub::setEnabled(bool enable)
{
    led_text->setEnabled(enable);
    pub_file->setEnabled(enable);
}

void LedPub::slot_load_file()
{
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

void LedPub::init()
{
    initElements();
    initGUI();
    initControl();
}

void LedPub::initElements()
{
    led_text = new QLineEdit();
    led_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pub_file = new QPushButton("...");
    pub_file->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
    pub_file->setMaximumWidth(30);
}

void LedPub::initGUI()
{
    QHBoxLayout* layout = new QHBoxLayout();

    layout->addWidget(led_text);
    layout->addWidget(pub_file);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
}

void LedPub::initControl()
{
    connect(pub_file, SIGNAL(pressed()), this, SLOT(slot_load_file()));
}
