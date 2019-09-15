/**
 * @file    report_view.h
 * @author  Thomas Mueller
 *
 * @brief  Dialog for the report output.
 *
 * This file is part of GeodesicView.
 */
#ifndef REPORT_VIEW_H
#define REPORT_VIEW_H

#include <iostream>

#include <QDialog>
#include <QPushButton>
#include <QTextEdit>

#include <gdefs.h>

/**
 * @brief The ReportDialog class
 */
class ReportDialog : public QDialog {
    Q_OBJECT

public:
    ReportDialog(QWidget* parent = nullptr);
    ~ReportDialog();

public:
    void setText(std::string text);

public slots:
    void slot_close();

protected:
    void init();
    void initElements();
    void initGUI();
    void initControl();

private:
    QPushButton* pub_close;
    QTextEdit* ted_report;
};

#endif
