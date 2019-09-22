/**
 * @file    prot_view.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>

#include "prot_view.h"

ProtDialog::ProtDialog(QWidget* parent)
    : QWidget(parent)
{
    QDir cdir = QDir::current();
    cdir.cd(DEF_STANDARD_DATA_DIRECTORY);
    mDir = cdir.absolutePath();

    init();
}

ProtDialog::~ProtDialog() {}

QString ProtDialog::getDirName()
{
    return lep_dir->getName();
}

QString ProtDialog::getBaseFilename()
{
    return lep_filename->getName();
}

bool ProtDialog::doWrite3dImage()
{
    return chb_save_3dimage->isChecked();
}

bool ProtDialog::doWrite2dImage()
{
    return chb_save_2dimage->isChecked();
}

bool ProtDialog::doWriteGeodesic()
{
    return chb_save_geodesic->isChecked();
}

bool ProtDialog::doWriteViewParams()
{
    return chb_save_viewparams->isChecked();
}

bool ProtDialog::doWriteParameters()
{
    return chb_save_parameters->isChecked();
}

bool ProtDialog::doWriteReport()
{
    return chb_save_report->isChecked();
}

bool ProtDialog::doWriteObjects()
{
    return chb_save_objects->isChecked();
}

void ProtDialog::slot_takeDir()
{
    mDir = lep_dir->getName();
    lep_filename->setDirPath(mDir);
}

void ProtDialog::slot_write()
{
    QString dirname = getDirName();
    QString basefilename = getBaseFilename();

    if (dirname == QString() || basefilename == QString()) {
        hide();
        return;
    }
    basefilename.remove(dirname);
    basefilename.remove(".2d.ppm");
    basefilename.remove(".points");
    basefilename.remove(".3d.ppm");
    basefilename.remove(DEF_REPORT_FILE_ENDING);
    basefilename.remove(DEF_PROTOCOL_FILE_ENDING);
    basefilename.remove(DEF_OBJECT_FILE_ENDING);

    QString filename = dirname + "/" + basefilename;
    filename.append(DEF_PROTOCOL_FILE_ENDING);
    if (QFile::exists(filename)) {
        int button = QMessageBox::question(this, "File exists", "File already exists. Overwrite?",
            QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Ok);
        if (button != QMessageBox::Ok) {
            return;
        }
    }

    emit emitWriteProt();
    hide();
}

void ProtDialog::slot_close()
{
    hide();
}

void ProtDialog::init()
{
    initElements();
    initGUI();
    initControl();
}

void ProtDialog::initElements()
{
    lab_dir = new QLabel("Directory");
    lep_dir = new LedPub(true);
    lep_dir->setDirPath(mDir);
    lep_dir->setName(mDir);

    lab_filename = new QLabel("Name");
    lep_filename = new LedPub();
    lep_filename->setDirPath(mDir);

    chb_save_3dimage = new QCheckBox("3d Image");
    chb_save_3dimage->setCheckState(Qt::Unchecked);

    chb_save_2dimage = new QCheckBox("2d Image");
    chb_save_2dimage->setCheckState(Qt::Unchecked);

    chb_save_geodesic = new QCheckBox("Geodesic");
    chb_save_geodesic->setCheckState(Qt::Unchecked);

    chb_save_viewparams = new QCheckBox("ViewParameters");
    chb_save_viewparams->setCheckState(Qt::Checked);

    chb_save_parameters = new QCheckBox("Parameters");
    chb_save_parameters->setCheckState(Qt::Checked);

    chb_save_report = new QCheckBox("Report");
    chb_save_report->setCheckState(Qt::Unchecked);

    chb_save_objects = new QCheckBox("Objects");
    chb_save_objects->setCheckState(Qt::Checked);

    pub_write = new QPushButton("Write");
    pub_cancel = new QPushButton("Cancel");
}

void ProtDialog::initGUI()
{
    QGroupBox* grb_dirname = new QGroupBox("Choose directory and base filename");
    QGridLayout* layout_dirname = new QGridLayout();
    layout_dirname->addWidget(lab_dir, 0, 0);
    layout_dirname->addWidget(lep_dir, 0, 1);
    layout_dirname->addWidget(lab_filename, 1, 0);
    layout_dirname->addWidget(lep_filename, 1, 1);
    grb_dirname->setLayout(layout_dirname);

    QGroupBox* grb_save = new QGroupBox("Protocol includes...");
    QGridLayout* layout_save = new QGridLayout();
    layout_save->addWidget(chb_save_parameters, 0, 0);
    layout_save->addWidget(chb_save_viewparams, 0, 1);
    layout_save->addWidget(chb_save_objects, 0, 2);
    layout_save->addWidget(chb_save_geodesic, 0, 3);
    layout_save->addWidget(chb_save_3dimage, 1, 0);
    layout_save->addWidget(chb_save_2dimage, 1, 1);
    layout_save->addWidget(chb_save_report, 1, 2);
    grb_save->setLayout(layout_save);

    QGridLayout* layout_complete = new QGridLayout();
    layout_complete->addWidget(grb_dirname, 0, 0, 1, 2);
    layout_complete->addWidget(grb_save, 1, 0, 1, 2);
    layout_complete->addWidget(pub_write, 2, 0);
    layout_complete->addWidget(pub_cancel, 2, 1);
    setLayout(layout_complete);

    setGeometry(0, 0, 500, 250);
    setWindowTitle("GeodesicViewer - Write protocol");
}

void ProtDialog::initControl()
{
    connect(lep_dir, SIGNAL(dirAccepted()), this, SLOT(slot_takeDir()));
    connect(pub_write, SIGNAL(pressed()), this, SLOT(slot_write()));
    connect(pub_cancel, SIGNAL(pressed()), this, SLOT(slot_close()));
}
