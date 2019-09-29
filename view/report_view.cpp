/**
 * @file    report_view.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "report_view.h"
#include <QGridLayout>

ReportDialog::ReportDialog(QWidget* parent)
    : QDialog(parent)
{
    init();
}

ReportDialog::~ReportDialog() {}

void ReportDialog::setText(std::string text)
{
    ted_report->clear();
    ted_report->setText(text.c_str());
}

void ReportDialog::slot_close()
{
    hide();
}

void ReportDialog::init()
{
    initElements();
    initGUI();
    initControl();

    setModal(false);
}

void ReportDialog::initElements()
{
    pub_close = new QPushButton("Close");

    ted_report = new QTextEdit();
    ted_report->setReadOnly(true);

    QFont mfont("Monospace", 10);
    ted_report->setFont(mfont);
}

void ReportDialog::initGUI()
{
    QGridLayout* layout_complete = new QGridLayout();
    layout_complete->addWidget(ted_report, 0, 0);
    layout_complete->addWidget(pub_close, 1, 0);
    setLayout(layout_complete);

    setGeometry(250, 150, 700, 300);
    setWindowTitle("GeodesicViewer - Report");
}

void ReportDialog::initControl()
{
    connect(pub_close, SIGNAL(pressed()), this, SLOT(slot_close()));
}
