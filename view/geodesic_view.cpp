/**
 * @file    geodesic_view.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "geodesic_view.h"
#include <QApplication>
#include <QDockWidget>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QScrollArea>

extern m4d::Object mObject;

#ifdef HAVE_LUA
#include "lua/m4dlua_metric.h"
#include "lua/m4dlua_solver.h"
#include "lua/m4dlua_utils.h"
#endif // HAVE_LUA

GeodesicView* GeodesicView::m_instance = nullptr;

GeodesicView::GeodesicView()
    : QMainWindow(nullptr)
{    
    mPreviousFolder = QApplication::applicationDirPath();

    setStandardParams(&mParams);
    init();
    // tab_draw->setCurrentIndex(1);
}

GeodesicView::~GeodesicView()
{
    if (!mPointData.empty()) {
        mPointData.clear();
    }
    if (!mDirData.empty()) {
        mDirData.clear();
    }
    if (!mEpsData.empty()) {
        mEpsData.clear();
    }

#ifdef HAVE_LUA
        //  lua_close(mLuaState);
#endif
}

QSize GeodesicView::minimumSizeHint() const
{
    return QSize(DEF_GUI_WIDTH, DEF_GUI_HEIGHT);
}

QSize GeodesicView::sizeHint() const
{
    return QSize(DEF_GUI_WIDTH, DEF_GUI_HEIGHT);
}

void GeodesicView::slot_load_setting()
{
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Load setting"), mPreviousFolder, tr(DEF_PROTOCOL_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (!mObject.loadSettings(filename.toStdString().c_str(), false)) {
        led_status->setText("load settings failed");
    }
    else {
        setSetting();
        calculateGeodesic();
    }
}

void GeodesicView::slot_save_setting()
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save setting"), mPreviousFolder, tr(DEF_PROTOCOL_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (!filename.endsWith(DEF_PROTOCOL_FILE_ENDING)) {
        filename.append(DEF_PROTOCOL_FILE_ENDING);
    }

    saveSetting(filename);
}

void GeodesicView::slot_load_vparams()
{
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Load ViewParameters"), mPreviousFolder, tr(DEF_VPARAMS_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (loadParamFile(filename.toStdString(), &mParams)) {
        opengl->updateParams();
        drw_view->updateParams();
        geo_view->updateParams();

        draw2d->updateParams();
        calculateGeodesic();
    }
}

void GeodesicView::slot_save_vparams()
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save ViewParameters"), mPreviousFolder, tr(DEF_VPARAMS_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (!filename.endsWith(DEF_VPARAMS_FILE_ENDING)) {
        filename.append(DEF_VPARAMS_FILE_ENDING);
    }

    saveParamFile(filename.toStdString(), &mParams);
}

void GeodesicView::slot_reset_vparams()
{
    mParams.opengl_projection = 0;
    mParams.opengl_draw3d_type = m4d::enum_draw_pseudocart;

    mParams.opengl_eye_pos = m4d::vec3(DEF_INIT_EYE_POS_X, DEF_INIT_EYE_POS_Y, DEF_INIT_EYE_POS_Z);
    mParams.opengl_eye_pos_step = 0.1;

    mParams.opengl_eye_poi = m4d::vec3(DEF_INIT_POI_X, DEF_INIT_POI_Y, DEF_INIT_POI_Z);
    mParams.opengl_eye_poi_step = 0.1;

    mParams.opengl_eye_vup = m4d::vec3(DEF_INIT_VUP_X, DEF_INIT_VUP_Y, DEF_INIT_VUP_Z);

    mParams.opengl_eye_dir = mParams.opengl_eye_poi - mParams.opengl_eye_pos;
    mParams.opengl_eye_dir.normalize();

    mParams.opengl_fov = DEF_INIT_FOV;
    mParams.opengl_fov_step = 0.1;

    mParams.opengl_stereo_use = 0;
    mParams.opengl_stereo_glasses = enum_stereo_red_cyan;
    mParams.opengl_stereo_type = enum_stereo_off_axis;
    mParams.opengl_stereo_sep = DEF_STEREO_SEP;
    mParams.opengl_stereo_step = DEF_STEREO_STEP;

    mParams.opengl_fog_use = 0;
    mParams.opengl_fog_init = DEF_OPENGL_FOG_DENSITY_INIT;
    mParams.opengl_fog_step = DEF_OPENGL_FOG_STEP;

    mParams.opengl_line_color = QColor(DEF_OPENGL_LINE_COLOR);
    mParams.opengl_line_width = DEF_OPENGL_LINE_WIDTH;
    mParams.opengl_line_smooth = 0;
    mParams.opengl_bg_color = QColor(DEF_OPENGL_BG_COLOR);

    mParams.opengl_leg1_col1 = QColor(DEF_OPENGL_LEG1_COL1);
    mParams.opengl_leg1_col2 = QColor(DEF_OPENGL_LEG1_COL2);
    mParams.opengl_leg1_freq = DEF_OPENGL_LEG1_FREQ;
    mParams.opengl_leg2_col1 = QColor(DEF_OPENGL_LEG2_COL1);
    mParams.opengl_leg2_col2 = QColor(DEF_OPENGL_LEG2_COL2);
    mParams.opengl_leg2_freq = DEF_OPENGL_LEG1_FREQ;

    mParams.opengl_emb_color = QColor(DEF_OPENGL_EMB_COLOR);

    opengl->updateParams();
    drw_view->updateParams();
    geo_view->updateParams();
    calculateGeodesic();
}

void GeodesicView::slot_save_image_2d()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save 2D image"), mPreviousFolder, DEF_2D_FILE_FILTER);

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (filename.endsWith(DEF_2D_FILE_ENDING)) {
        filename.remove(DEF_2D_FILE_ENDING);
    }
    else if (filename.endsWith(DEF_3D_FILE_ENDING)) {
        filename.remove(DEF_3D_FILE_ENDING);
    }
    else if (filename.endsWith(DEF_IMG_FILE_ENDING)) {
        filename.remove(DEF_IMG_FILE_ENDING);
    }
    filename.append(DEF_2D_FILE_ENDING);

    int currIdx = tab_draw->currentIndex();
    tab_draw->setCurrentIndex(1);
    draw2d->saveRGBimage(filename);
    tab_draw->setCurrentIndex(currIdx);
}

void GeodesicView::slot_save_image_3d()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save 3D image"), mPreviousFolder, DEF_3D_FILE_FILTER);

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (filename.endsWith(DEF_3D_FILE_ENDING)) {
        filename.remove(DEF_3D_FILE_ENDING);
    }
    else if (filename.endsWith(DEF_2D_FILE_ENDING)) {
        filename.remove(DEF_2D_FILE_ENDING);
    }
    else if (filename.endsWith(DEF_IMG_FILE_ENDING)) {
        filename.remove(DEF_IMG_FILE_ENDING);
    }
    filename.append(DEF_3D_FILE_ENDING);

    int currIdx = tab_draw->currentIndex();
    tab_draw->setCurrentIndex(0);
    opengl->saveRGBimage(filename);
    tab_draw->setCurrentIndex(currIdx);
}

void GeodesicView::slot_load_all()
{
    QString filename
        = QFileDialog::getOpenFileName(this, tr("Load all"), mPreviousFolder, tr(DEF_PROTOCOL_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (!mObject.loadSettings(filename.toStdString().c_str(), false)) {
        led_status->setText("load settings failed");
    }
    else {
        QString basefilename = filename.remove(DEF_PROTOCOL_FILE_ENDING);
        // std::cerr << basefilename.toStdString() << std::endl;
        setSetting();

        QString paramFilename = basefilename + QString(DEF_VPARAMS_FILE_ENDING);
        // std::cerr << paramFilename.toStdString() << std::endl;
        if (loadParamFile(paramFilename.toStdString(), &mParams)) {
            drw_view->updateParams();
            geo_view->updateParams();
            opengl->updateParams();
            draw2d->updateParams();
        }

        QString objFilename = basefilename + QString(DEF_OBJECT_FILE_ENDING);
        loadObjects(objFilename);
        calculateGeodesic();
    }
}

void GeodesicView::slot_prot_dialog()
{
    QRect mg = geometry();
    QRect md = mProtDialog->geometry();

    mProtDialog->move(mg.center().x() - md.width() / 2, mg.center().y() - md.height() / 2);

    mProtDialog->setWindowModality(Qt::ApplicationModal);
    mProtDialog->show();
}

void GeodesicView::slot_write_prot()
{
    calculateGeodesicData();

    QString dirname = mProtDialog->getDirName();
    QString basefilename = mProtDialog->getBaseFilename();

    if (dirname == QString() || basefilename == QString()) {
        return;
    }

    basefilename.remove(dirname);
    basefilename.remove(".2d.ppm");
    basefilename.remove(".points");
    basefilename.remove(".3d.ppm");
    basefilename.remove(DEF_REPORT_FILE_ENDING);
    basefilename.remove(DEF_PROTOCOL_FILE_ENDING);
    basefilename.remove(DEF_VPARAMS_FILE_ENDING);
    basefilename.remove(DEF_OBJECT_FILE_ENDING);

    // ---------------------------------
    //    save settings
    // ---------------------------------
    if (mProtDialog->doWriteParameters()) {
        QString filename = dirname + "/" + basefilename;
        filename.append(DEF_PROTOCOL_FILE_ENDING);
        saveSetting(filename);
    }

    // ---------------------------------
    //  save view parameters
    // ---------------------------------
    if (mProtDialog->doWriteViewParams()) {
        QString filename = dirname + "/" + basefilename;
        filename.append(DEF_VPARAMS_FILE_ENDING);
        saveParamFile(filename.toStdString(), &mParams);
    }

    // ---------------------------------
    //  save objects
    // ---------------------------------
    if (mProtDialog->doWriteObjects()) {
        QString filename = dirname + "/" + basefilename;
        filename.append(DEF_OBJECT_FILE_ENDING);
        saveObjects(filename);
    }

    // ---------------------------------
    //   save geodesic data
    // ---------------------------------
    if (mProtDialog->doWriteGeodesic() && mPointData.size() > 0) {
        QString filename = dirname + "/" + basefilename + ".points";
        QFile out_points(filename);
        if (!out_points.open(QFile::WriteOnly | QFile::Text)) {
            QString msg = QString("Cannot open file %1 for output!").arg(filename);
            QMessageBox::warning(this, "Error", msg);
            return;
        }

        QTextStream ts_points(&out_points);
        for (unsigned int i = 0; i < mPointData.size(); i++) {
            char buf[256];
            //   sprintf(buf,"%5d %12.8f %12.8f %12.8f %12.8f   %12.8f %12.8f %12.8f %12.8f  %12.8g\n",i,
            //           mPointData[i][0],mPointData[i][1],mPointData[i][2],mPointData[i][3],mPointData[i][4],mPointData[i][5],mPointData[i][6],mPointData[i][7],mPointData[i][8]);
#ifdef _WIN32
            sprintf_s(buf, "%5d %16.12f %16.12f %16.12f %16.12f   %16.12f %16.12f %16.12f %16.12f  %16.12g\n", i,
                mPointData[i][0], mPointData[i][1], mPointData[i][2], mPointData[i][3], mDirData[i][0], mDirData[i][1],
                mDirData[i][2], mDirData[i][3], mEpsData[i]);
#else
            sprintf(buf, "%5d %16.12f %16.12f %16.12f %16.12f   %16.12f %16.12f %16.12f %16.12f  %16.12g\n", i,
                mPointData[i][0], mPointData[i][1], mPointData[i][2], mPointData[i][3], mDirData[i][0], mDirData[i][1],
                mDirData[i][2], mDirData[i][3], mEpsData[i]);
#endif
            ts_points << buf;
        }
        out_points.close();
    }

    // ---------------------------------
    //    save images
    // ---------------------------------
    int currIdx = tab_draw->currentIndex();
    if (mProtDialog->doWrite3dImage()) {
        tab_draw->setCurrentIndex(0);
        QString filename = dirname + "/" + basefilename + DEF_3D_FILE_ENDING;
        opengl->saveRGBimage(filename);
    }
    if (mProtDialog->doWrite2dImage()) {
        tab_draw->setCurrentIndex(1);
        QString filename = dirname + "/" + basefilename + DEF_2D_FILE_ENDING;
        draw2d->saveRGBimage(filename);
    }
    tab_draw->setCurrentIndex(currIdx);

    // ---------------------------------
    //  save report
    // ---------------------------------
    if (mProtDialog->doWriteReport()) {
        QString filename = dirname + "/" + basefilename;
        filename.append(DEF_REPORT_FILE_ENDING);
        saveReport(filename);
    }
}

void GeodesicView::slot_quit()
{
    close();
}

#ifdef HAVE_LUA
void GeodesicView::slot_run_luaScript()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Load Lua Script"), mPreviousFolder, tr("*.lua"));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (luaL_loadfile(mLuaState, filename.toStdString().c_str()) || lua_pcall(mLuaState, 0, 0, 0)) {
        mlua_error(mLuaState, "Error: %s\n", lua_tostring(mLuaState, -1));
        fprintf(stderr, "Not all parameters could be set correctly!\n");
        return;
    }

    setSetting();

    if (tab_draw->currentIndex() == 1) { // set points for 2D visualization...
        int absIdx, ordIdx;
        drw_view->getDrawAbsOrdIndex(absIdx, ordIdx);
        draw2d->setPoints(mObject.currMetric->getCurrDrawType(drw_view->getDrawTypeName()));
    }
    else if (tab_draw->currentIndex() == 0) { // set sachs axes and points for 3D visualization...
        if (mObject.type == m4d::enum_geodesic_lightlike_sachs) {
            opengl->setSachsAxes(false);
        }
        else {
            opengl->clearSachsAxes();
        }
        opengl->setPoints(mObject.currMetric->getCurrDrawType(drw_view->getDrawType3DName()));
    }
}
#endif // HAVE_LUA

void GeodesicView::reset()
{
    cob_metric->setCurrentIndex(0);
    slot_setCurrentMetric();
    setMetricNamesAndCoords();

    cob_integrator->setCurrentIndex(0);
    slot_setGeodSolver();

    chb_stepsize_controlled->setChecked(false);
    chb_stepsize_controlled->setEnabled(false);
    led_eps_abs->setText(QString::number(1.0e-6));
    led_eps_abs->setEnabled(false);
    led_eps_rel->setText(QString::number(0.0));
    led_eps_rel->setEnabled(false);
    led_max_stepsize->setEnabled(false);
    led_constraint_eps->setText(QString::number(DEF_CONSTRAINT_EPSILON));

    setStandardParams(&mParams);

    mObjectView->clearAll();
    geo_view->resetAll();

    opengl->reset();
    draw2d->reset();
    mOglJacobi->reset();

    lct_view->resetAll();
    drw_view->resetAll();

    mObject.resetAll();
}

void GeodesicView::slot_reset()
{
    int button = QMessageBox::question(
        this, "Reset", "Really reset GeodesicViewer?", QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Ok);
    if (button != QMessageBox::Ok) {
        return;
    }

    reset();
}

void GeodesicView::slot_doc()
{
    // if (!mHelpDoc->isVisible()) {
    //    mHelpDoc->show();
    //} else {
    //    mHelpDoc->hide();
    //    mHelpDoc->show();
    //}
}

void GeodesicView::slot_about()
{
    QMessageBox::about(this, tr("GeodesicViewer"),
        tr("GeodesicViewer was first developed by Thomas Müller and Frank Grave at "
           "the Visualization Research Center, University of Stuttgart, Germany (2009-2015).\n\n"
           "Current contact:\nDr. Thomas Müller\nHaus der Astronomie/Max Planck Institute for Astronomy\n69117 "
           "Heidelberg, Germany\n\n"
           "Email: tmueller@mpia.de\n\n"
           "Details about the metrics can be found in the\n\"Catalogue of Spacetimes\", arXiv:0904.4184 [gr-qc]"));
}

void GeodesicView::slot_changeDrawActive()
{
    QObject* obj = sender();
    if (obj->objectName() == tr("action3dactive")) {
        tab_draw->setCurrentIndex(0);
        opengl->setFocus();
    }
    else {
        drw_view->animate(false);
        tab_draw->setCurrentIndex(1);
        draw2d->setFocus();
    }
}

void GeodesicView::slot_animate()
{
    if (drw_view->isAnimated()) {
        drw_view->animate(false);
    }
    else {
        drw_view->animate(true);
    }
}

void GeodesicView::slot_load_objects()
{
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Load object file"), mPreviousFolder, tr(DEF_OBJECT_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    loadObjects(filename);
}

void GeodesicView::slot_append_objects()
{
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Append object file"), mPreviousFolder, tr(DEF_OBJECT_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    opengl->clearAllObjects();
    draw2d->clearAllObjects();

    readObjectFile(filename.toStdString(), mObjects, false);
    size_t num = mObjects.size();

    for (size_t i = 0; i < num; i++) {
        if (mObjects[i]->getObjectDim() == enum_object_dim_3d) {
            opengl->insertObject(mObjects[i]);
        }
        else if (mObjects[i]->getObjectDim() == enum_object_dim_2d) {
            draw2d->insertObject(mObjects[i]);
        }
    }
}

void GeodesicView::slot_save_objects()
{
    if (mObjects.empty()) {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save object file"), mPreviousFolder, tr(DEF_OBJECT_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (!filename.endsWith(DEF_OBJECT_FILE_ENDING)) {
        filename.append(DEF_OBJECT_FILE_ENDING);
    }

    saveObjects(filename);
}

void GeodesicView::slot_show_objects()
{
    if (mObjectView->isVisible()) {
        mObjectView->hide();
    }
    else {
        QString text;
        for (unsigned int i = 0; i < mObjects.size(); i++) {
            std::string line;
            mObjects[i]->printLine(line);
            // mObjects[i]->print();
            text.append(QString("%1\n").arg(QString(line.c_str())));
        }
        mObjectView->writeText(text);
        mObjectView->show();
    }
}

void GeodesicView::slot_objChanged()
{
    QString text = mObjectView->getText();
    std::vector<std::vector<std::string>> tokens;

    QTextStream ts(&text);
    while (!ts.atEnd()) {
        QString line = ts.readLine();
        QStringList sl = line.split(" ", QString::SkipEmptyParts);

        std::vector<std::string> sline;
        for (int i = 0; i < sl.size(); i++) {
            sline.push_back(sl[i].toStdString());
        }

        if (sline.size() > 0) {
            tokens.push_back(sline);
        }
    }

    if (!mObjects.empty()) {
        mObjects.clear();
    }

    opengl->clearAllObjects();
    draw2d->clearAllObjects();
    size_t num = static_cast<size_t>(readObjectsFromTokens(tokens, mObjects));

    for (size_t i = 0; i < num; i++) {
        if (mObjects[i]->getObjectDim() == enum_object_dim_3d) {
            opengl->insertObject(mObjects[i]);
        }
        else if (mObjects[i]->getObjectDim() == enum_object_dim_2d) {
            draw2d->insertObject(mObjects[i]);
        }
    }
}

void GeodesicView::slot_show_report()
{
    char* text = nullptr;
    mObject.makeReport(text);
    if (text != nullptr) {
        mReportText->setText(text);
        mReportText->show();
    }
    SafeDelete<char>(text);
}

void GeodesicView::slot_save_report()
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save report"), mPreviousFolder, tr(DEF_REPORT_FILE_ENDING_PATTERN));

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();

    if (!filename.endsWith(DEF_REPORT_FILE_ENDING)) {
        filename.append(DEF_REPORT_FILE_ENDING);
    }

    saveReport(filename);
}

void GeodesicView::slot_setCurrentMetric()
{
    int index = cob_metric->currentIndex();
    if (!setMetric(m4d::MetricList::enum_metric(index))) {
        return;
    }

    if (!setMetricNamesAndCoords()) {
        return;
    }

    lct_view->adjustLocTedTypes();
    drw_view->adjustDrawTypes();
    drw_view->adjustEmbParams();

    opengl->clearEmbed();

    /* --- set start position --- */
    m4d::vec4 pos = mObject.currMetric->getStandardInitialPosition();
    lct_view->adjustMetric();

    mObject.startPos = pos;
    mObject.startDir = mObject.currMetric->getStandardInitialDirection();

    mObject.ksi = DEF_INIT_KSI;
    mObject.chi = DEF_INIT_CHI;
    mObject.resetLorentzTransf();
    mObject.type = m4d::enum_geodesic_lightlike;

    geo_view->resetAll();
    lct_view->setLocalTetrad(true);
    lct_view->resetBoost();

    cob_integrator->setCurrentIndex(0);
    setGeodSolver(m4d::enum_integrator(0));

    if (mObject.geodSolver != nullptr) {
        mObject.geodSolver->setMetric(mObject.currMetric);
        calculateGeodesic();
    }

    led_speed_of_light->setText(QString::number(1.0));
    led_grav_constant->setText(QString::number(1.0));
}

void GeodesicView::slot_metricParamChanged()
{
    QObject* obj = sender();
    DoubleEdit* led = reinterpret_cast<DoubleEdit*>(obj);
    double value = led->getValue();

    // std::cerr << obj->objectName().toStdString() << " " << value << std::endl;

    QString paramName = QString(obj->objectName()).replace(QString(".value"), QString(""));

    if (mObject.currMetric != nullptr) {
        mObject.currMetric->setParam(paramName.toStdString().c_str(), value);
        calculateGeodesic();

        if (mObject.currMetric->getCurrDrawType(drw_view->getDrawType3DName()) == m4d::enum_draw_embedding) {
            opengl->genEmbed(mObject.currMetric);
        }
    }
}

void GeodesicView::slot_setGeodSolver()
{
    int index = cob_integrator->currentIndex();
    setGeodSolver(m4d::enum_integrator(index));
    calculateGeodesic();
}

void GeodesicView::slot_setMaxNumPoints()
{
    mObject.maxNumPoints = led_maxpoints->text().toUInt();
    if (mObject.maxNumPoints <= 10) {
        mObject.maxNumPoints = 10;
        led_maxpoints->setText(QString::number(mObject.maxNumPoints));
    }
    calculateGeodesic();
}

void GeodesicView::slot_setStepsizeControl()
{
    if (mObject.geodSolver == nullptr) {
        return;
    }
    if (chb_stepsize_controlled->checkState() == Qt::Checked) {
        mObject.geodSolver->setStepSizeControlled(true);
        mObject.stepsizeControlled = true;
    }
    else {
        mObject.geodSolver->setStepSizeControlled(false);
        mObject.stepsizeControlled = false;
    }
    calculateGeodesic();
}

void GeodesicView::slot_setIntegratorParams()
{
    mObject.stepsize = led_stepsize->text().toDouble();
    mObject.epsAbs = led_eps_abs->text().toDouble();
    mObject.epsRel = led_eps_rel->text().toDouble();

    mObject.geodSolver->setEpsilons(mObject.epsAbs, mObject.epsRel);
    mObject.geodSolver->setAffineParamStep(mObject.stepsize);
    calculateGeodesic();
}

void GeodesicView::slot_setDrawWithLines()
{
    if (chb_drawstyle->checkState() == Qt::Checked) {
        draw2d->setStyle(enum_draw_lines);
        opengl->setStyle(enum_draw_lines);
    }
    else {
        draw2d->setStyle(enum_draw_points);
        opengl->setStyle(enum_draw_points);
    }
    calculateGeodesic();
}

void GeodesicView::slot_setConstraintEps()
{
    double eps = led_constraint_eps->text().toDouble();
    mObject.geodSolver->setConstrEps(eps);
    calculateGeodesic();
}

void GeodesicView::slot_setResize()
{
    double ce = led_constraint_eps->text().toDouble();
    double eps = led_resize_eps->text().toDouble();
    double fac = led_resize_fac->text().toDouble();

    if (eps >= ce) {
        fac = 1.0;
        led_resize_fac->setText(QString::number(fac));
        led_resize_fac->setEnabled(false);
    }
    else {
        led_resize_fac->setEnabled(true);
    }

    mObject.geodSolver->setResize(eps, fac);
    calculateGeodesic();
}

void GeodesicView::slot_setMaxStepSize()
{
    mObject.geodSolver->setMaxAffineParamStep(led_max_stepsize->text().toDouble());
    calculateGeodesic();
}

void GeodesicView::slot_setMinStepSize()
{
    mObject.geodSolver->setMinAffineParamStep(led_min_stepsize->text().toDouble());
    calculateGeodesic();
}

void GeodesicView::slot_setSIunits()
{
    led_speed_of_light->setText(QString::number(M4D_SPEED_OF_LIGHT, 'f', 3));
    led_grav_constant->setText(QString::number(M4D_GRAV_CONST));
    led_dielectric_perm->setText(QString::number(M4D_DIELECTRIC_PERM));
    setUnits(M4D_SPEED_OF_LIGHT, M4D_GRAV_CONST, M4D_DIELECTRIC_PERM);
}

void GeodesicView::slot_setGeomUnits()
{
    led_speed_of_light->setText(QString::number(1.0));
    led_grav_constant->setText(QString::number(1.0));
    led_dielectric_perm->setText(QString::number(1.0));
    setUnits(1.0, 1.0, 1.0);
}

void GeodesicView::slot_setUnits()
{
    double sol = led_speed_of_light->text().toDouble();
    double grav = led_grav_constant->text().toDouble();
    double dipm = led_dielectric_perm->text().toDouble();
    setUnits(sol, grav, dipm);
}

void GeodesicView::slot_calcGeodesic()
{
    calculateGeodesic();
}

void GeodesicView::slot_setOpenGLcolors()
{
    QColor bgcol, fgcol;

    drw_view->getColors(bgcol, fgcol);
    opengl->setFGcolor(fgcol);
    opengl->setBGcolor(bgcol);
}

void GeodesicView::slot_executeScript()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Load script file"), mPreviousFolder, "*.js");

    if (filename == QString()) {
        return;
    }

    mPreviousFolder = QFileInfo(filename).absoluteDir().absolutePath();
    execScript(filename);
}

void GeodesicView::execScript(QString filename)
{
    QFile scriptFile(filename);
    scriptFile.open(QIODevice::ReadOnly);
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();

    // QScriptValue result = mScriptEngine->evaluate(contents, filename);
    // if (result.isError()) {
    //     QMessageBox::critical(0, "Executing QtScript",
    //                           QString::fromLatin1("%0:%1: %2")
    //                           .arg(filename)
    //                           .arg(result.property("lineNumber").toInt32())
    //                           .arg(result.toString()));
    // }
}

void GeodesicView::setMetric(QString name)
{
    int index = mObject.metricDB.getMetricNr(name.toStdString().c_str());
    if (index == static_cast<int>(m4d::MetricList::enum_metric_unknown)) {
        return;
    }
    cob_metric->setCurrentIndex(index);
    slot_setCurrentMetric();
}

void GeodesicView::setMetricParam(int num, double val)
{
    if (mObject.currMetric != nullptr) {
        int numParams = mObject.currMetric->getNumParams();
        if (num >= 0 && num < numParams) {
            DoubleEdit* led_value = reinterpret_cast<DoubleEdit*>(tbw_metric_params->cellWidget(num, 1));
            led_value->setValue(val);
            mObject.currMetric->setParam(num, val);
            calculateGeodesic();

            if (mObject.currMetric->getCurrDrawType(drw_view->getDrawType3DName()) == m4d::enum_draw_embedding) {
                opengl->genEmbed(mObject.currMetric);
            }
        }
    }
}

void GeodesicView::setMetricParam(QString name, double val)
{
    if (mObject.currMetric != nullptr) {
        int paramNum = mObject.currMetric->getParamNum(name.toStdString().c_str());
        if (paramNum < 0) {
            fprintf(stderr,
                "GeodesicView::setMetricParam() ... parameter name \"%s\" does not exist for the current metric!\n",
                name.toStdString().c_str());
            return;
        }
        DoubleEdit* led_value = reinterpret_cast<DoubleEdit*>(tbw_metric_params->cellWidget(paramNum, 1));
        led_value->setValue(val);
        mObject.currMetric->setParam(name.toStdString().c_str(), val);
        calculateGeodesic();

        if (mObject.currMetric->getCurrDrawType(drw_view->getDrawType3DName()) == m4d::enum_draw_embedding) {
            opengl->genEmbed(mObject.currMetric);
        }
    }
}

void GeodesicView::setSolver(QString name)
{
    unsigned int num = 0;
    while (num < m4d::NUM_GEOD_SOLVERS) {
        if (name.compare(QString(m4d::stl_solver_nicknames[num])) == 0) {
            cob_integrator->setCurrentIndex(static_cast<int>(num));
            slot_setGeodSolver();
            break;
        }
        num++;
    }
}

void GeodesicView::setSolverParam(QString name, double val)
{
    if (name.compare("max points") == 0) {
        led_maxpoints->setText(QString::number(val));
        slot_setMaxNumPoints();
    }
    else if (name.compare("step size") == 0) {
        led_stepsize->setText(QString::number(val));
        slot_setIntegratorParams();
    }
    else if (name.compare("eps abs") == 0) {
        led_eps_abs->setText(QString::number(val));
        slot_setIntegratorParams();
    }
    else if (name.compare("eps rel") == 0) {
        led_eps_rel->setText(QString::number(val));
        slot_setIntegratorParams();
    }
    else if (name.compare("step ctrl") == 0) {
        if (val > 0) {
            chb_stepsize_controlled->setChecked(true);
        }
        else {
            chb_stepsize_controlled->setChecked(false);
        }
        slot_setStepsizeControl();
    }
    else if (name.compare("constr eps") == 0) {
        led_constraint_eps->setText(QString::number(val));
        slot_setConstraintEps();
    }
    else {
        fprintf(
            stderr, "GeodesicView::setSolverParam() ... parameter \"%s\" is not valid.\n", name.toStdString().c_str());
        fprintf(stderr,
            "Use: \"max points\", \"step ctrl\", \"step size\", \"eps abs\", \"eps.rel\", or \"constr eps\".\n");
    }
}

void GeodesicView::wait(double)
{
#ifndef _WIN32
// usleep(sec*1e6);
#endif
}

void GeodesicView::addObject(MyObject* obj)
{
    mObjects.push_back(obj);
    if (obj->getObjectDim() == enum_object_dim_3d) {
        opengl->insertObject(obj);
    }
    else if (obj->getObjectDim() == enum_object_dim_2d) {
        draw2d->insertObject(obj);
    }
}

void GeodesicView::clearAllObjects()
{
    if (!mObjects.empty()) {
        mObjects.clear();
    }

    opengl->clearAllObjects();
    draw2d->clearAllObjects();
}

void GeodesicView::init()
{
    initGUI();

    initControl();
    initActions();
    initMenus();

    initScripting();
    initStatusTips();

#ifdef HAVE_NETWORK
    mServer = new QTcpServer(this);
    mServer->listen(QHostAddress::Any, DEF_SOCKET_PORT);
    connect(mServer, SIGNAL(newConnection()), this, SLOT(slot_newConnect()));
#endif // HAVE_NETWORK

#ifdef HAVE_LUA
    // Initialize lua stuff
    mLuaState = luaL_newstate();
    luaL_openlibs(mLuaState);

    // register all functions
    lua_reg_metric(mLuaState);
    lua_reg_solver(mLuaState);
    lua_reg_utils(mLuaState);

    lua_register(mLuaState, "ClearAllGVObjects", lclearAllGVObjects);
    lua_register(mLuaState, "AddGVObject", laddGVObject);

    lua_register(mLuaState, "SetGVCamera", setGVCamera);
#endif

    setWindowTitle("GeodesicViewer");
}

void GeodesicView::initActions()
{
    /* ------------------
     *  File actions
     * ------------------ */
    mActionReset = new QAction(QIcon(":/reset.png"), "&Reset", this);
    mActionReset->setShortcut(Qt::CTRL | Qt::Key_N);
    addAction(mActionReset);
    connect(mActionReset, SIGNAL(triggered()), this, SLOT(slot_reset()));

    mActionLoadSetting = new QAction(QIcon(":/load.png"), "&Load Setting", this);
    mActionLoadSetting->setShortcut(Qt::CTRL | Qt::Key_L);
    addAction(mActionLoadSetting);
    connect(mActionLoadSetting, SIGNAL(triggered()), this, SLOT(slot_load_setting()));

    mActionSaveSetting = new QAction(QIcon(":/save.png"), "&Save Setting", this);
    mActionSaveSetting->setShortcut(Qt::CTRL | Qt::Key_S);
    addAction(mActionSaveSetting);
    connect(mActionSaveSetting, SIGNAL(triggered()), this, SLOT(slot_save_setting()));

    mActionLoadViewParams = new QAction(QIcon(":/load.png"), "Load ViewParams", this);
    mActionLoadViewParams->setShortcut(Qt::CTRL | Qt::Key_V);
    addAction(mActionLoadViewParams);
    connect(mActionLoadViewParams, SIGNAL(triggered()), this, SLOT(slot_load_vparams()));

    mActionSaveViewParams = new QAction(QIcon(":/save.png"), "Save ViewParams", this);
    addAction(mActionSaveViewParams);
    connect(mActionSaveViewParams, SIGNAL(triggered()), this, SLOT(slot_save_vparams()));

    mActionResetViewParams = new QAction("Reset ViewParams", this);
    addAction(mActionResetViewParams);
    connect(mActionResetViewParams, SIGNAL(triggered()), this, SLOT(slot_reset_vparams()));

    mActionLoadAll = new QAction(QIcon(":/load.png"), "Load all (ini,cfg,obj)", this);
    mActionLoadAll->setShortcut(Qt::CTRL | Qt::Key_A);
    addAction(mActionLoadAll);
    connect(mActionLoadAll, SIGNAL(triggered()), this, SLOT(slot_load_all()));

    mActionSaveImage2D = new QAction(QIcon(":/save.png"), "Save Image &2D", this);
    addAction(mActionSaveImage2D);
    connect(mActionSaveImage2D, SIGNAL(triggered()), this, SLOT(slot_save_image_2d()));

    mActionSaveImage3D = new QAction(QIcon(":/save.png"), "Save Image &3D", this);
    addAction(mActionSaveImage3D);
    connect(mActionSaveImage3D, SIGNAL(triggered()), this, SLOT(slot_save_image_3d()));

    mActionWriteProtocoll = new QAction(QIcon(":/write.png"), "&Write Protocol", this);
    mActionWriteProtocoll->setShortcut(Qt::CTRL | Qt::Key_W);
    addAction(mActionWriteProtocoll);
    connect(mActionWriteProtocoll, SIGNAL(triggered()), this, SLOT(slot_prot_dialog()));

    mActionQuit = new QAction(QIcon(":/exit.png"), "&Quit", this);
    mActionQuit->setShortcut(Qt::CTRL | Qt::Key_Q);
    addAction(mActionQuit);
    connect(mActionQuit, SIGNAL(triggered()), this, SLOT(slot_quit()));

#ifdef HAVE_LUA
    mActionRunLuaScript = new QAction("&Run Lua Script", this);
    mActionRunLuaScript->setShortcut(Qt::CTRL | Qt::Key_Y);
    addAction(mActionRunLuaScript);
    connect(mActionRunLuaScript, SIGNAL(triggered()), this, SLOT(slot_run_luaScript()));
#endif // HAVE_LUA

    /* ------------------
     *  Object actions
     * ------------------ */
    mActionLoadObjectFile = new QAction(QIcon(":/load.png"), "&Load object file", this);
    mActionLoadObjectFile->setShortcut(Qt::CTRL | Qt::Key_O);
    addAction(mActionLoadObjectFile);
    connect(mActionLoadObjectFile, SIGNAL(triggered()), this, SLOT(slot_load_objects()));

    mActionAppendObjectFile = new QAction(QIcon(":/add.png"), "&Append object file", this);
    addAction(mActionAppendObjectFile);
    connect(mActionAppendObjectFile, SIGNAL(triggered()), this, SLOT(slot_append_objects()));

    mActionSaveObjectFile = new QAction(QIcon(":/save.png"), "&Save object file", this);
    addAction(mActionSaveObjectFile);
    connect(mActionSaveObjectFile, SIGNAL(triggered()), this, SLOT(slot_save_objects()));

    mActionShowObjects = new QAction(QIcon(":/display.png"), "&Show objects", this);
    mActionShowObjects->setShortcut(Qt::CTRL | Qt::ShiftModifier | Qt::Key_O);
    mActionShowObjects->setShortcutContext(Qt::ApplicationShortcut);
    addAction(mActionShowObjects);
    connect(mActionShowObjects, SIGNAL(triggered()), this, SLOT(slot_show_objects()));

    // ------------------
    //  Report actions
    // ------------------
    mActionShowReport = new QAction(QIcon(":/display.png"), "&Show report", this);
    mActionShowReport->setShortcut(Qt::CTRL | Qt::Key_R);
    addAction(mActionShowReport);
    connect(mActionShowReport, SIGNAL(triggered()), this, SLOT(slot_show_report()));
    //
    // mActionSaveReport = new QAction(QIcon(":/save.png"), "&Save report", this);
    // addAction(mActionSaveReport);
    // connect(mActionSaveReport, SIGNAL(triggered()), this, SLOT(slot_save_report()));

    // ------------------
    //   Help actions
    // ------------------
    // mActionDoc = new QAction(QIcon(":/html.png"), "&Manual", this);
    // mActionDoc->setShortcut(Qt::CTRL | Qt::Key_M);
    // addAction(mActionDoc);
    // connect(mActionDoc, SIGNAL(triggered()), this, SLOT(slot_doc()));

    mActionAbout = new QAction(QIcon(":/about.png"), "&About", this);
    addAction(mActionAbout);
    connect(mActionAbout, SIGNAL(triggered()), this, SLOT(slot_about()));

    // ------------------
    //   other actions
    // ------------------
    mActionMake3Dactive = new QAction(this);
    mActionMake3Dactive->setShortcut(tr("Ctrl+1"));
    mActionMake3Dactive->setObjectName(tr("action3dactive"));
    addAction(mActionMake3Dactive);
    connect(mActionMake3Dactive, SIGNAL(triggered()), this, SLOT(slot_changeDrawActive()));

    mActionMake2Dactive = new QAction(this);
    mActionMake2Dactive->setShortcut(tr("Ctrl+2"));
    mActionMake2Dactive->setObjectName(tr("action2dactive"));
    addAction(mActionMake2Dactive);
    connect(mActionMake2Dactive, SIGNAL(triggered()), this, SLOT(slot_changeDrawActive()));

    mActionPlay = new QAction(this);
    mActionPlay->setShortcut(tr("Ctrl+P"));
    addAction(mActionPlay);
    connect(mActionPlay, SIGNAL(triggered()), this, SLOT(slot_animate()));

    mActionStepKsiPlus = new QAction(this);
    mActionStepKsiPlus->setShortcut(tr("Ctrl+Shift+K"));
    addAction(mActionStepKsiPlus);
    connect(mActionStepKsiPlus, SIGNAL(triggered()), geo_view, SLOT(slot_incrKsi()));

    mActionStepKsiMinus = new QAction(this);
    mActionStepKsiMinus->setShortcut(tr("Ctrl+K"));
    addAction(mActionStepKsiMinus);
    connect(mActionStepKsiMinus, SIGNAL(triggered()), geo_view, SLOT(slot_decrKsi()));

    mActionStepChiPlus = new QAction(this);
    mActionStepChiPlus->setShortcut(tr("Ctrl+Shift+C"));
    addAction(mActionStepChiPlus);
    connect(mActionStepChiPlus, SIGNAL(triggered()), geo_view, SLOT(slot_incrChi()));

    mActionStepChiMinus = new QAction(this);
    mActionStepChiMinus->setShortcut(tr("Ctrl+C"));
    addAction(mActionStepChiMinus);
    connect(mActionStepChiMinus, SIGNAL(triggered()), geo_view, SLOT(slot_decrChi()));

    mLoadScript = new QAction(this);
    mLoadScript->setShortcut(tr("Ctrl+Shift+B"));
    addAction(mLoadScript);
    connect(mLoadScript, SIGNAL(triggered()), this, SLOT(slot_executeScript()));
}

void GeodesicView::initMenus()
{
    // ---- File menu ----
    mFileMenu = menuBar()->addMenu("&File");
    mFileMenu->addAction(mActionReset);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mActionLoadSetting);
    mFileMenu->addAction(mActionSaveSetting);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mActionLoadViewParams);
    mFileMenu->addAction(mActionSaveViewParams);
    mFileMenu->addAction(mActionResetViewParams);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mActionLoadAll);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mActionSaveImage2D);
    mFileMenu->addAction(mActionSaveImage3D);
    mFileMenu->addAction(mActionWriteProtocoll);
#ifdef HAVE_LUA
    mFileMenu->addSeparator();
    mFileMenu->addAction(mActionRunLuaScript);
#endif
    mFileMenu->addSeparator();
    mFileMenu->addAction(mActionQuit);

    // ---- Object menu ----
    mObjectMenu = menuBar()->addMenu("&Objects");
    mObjectMenu->addAction(mActionLoadObjectFile);
    mObjectMenu->addAction(mActionAppendObjectFile);
    mObjectMenu->addAction(mActionSaveObjectFile);
    mObjectMenu->addSeparator();
    mObjectMenu->addAction(mActionShowObjects);

    // ---- Report menu ----
    // mReportMenu = menuBar()->addMenu("&Report");
    // mReportMenu->addAction(mActionSaveReport);
    // mReportMenu->addSeparator();
    // mReportMenu->addAction(mActionShowReport);

    // ---- Help menu ----
    mHelpMenu = menuBar()->addMenu("&Help");
    mHelpMenu->addAction(mActionShowReport);
    mHelpMenu->addSeparator();
    // mHelpMenu -> addAction(mActionDoc);
    mHelpMenu->addAction(mActionAbout);
}

void GeodesicView::initScripting()
{
    //  mScriptEngine = new QScriptEngine();
    //
    //  QScriptValue gv = mScriptEngine->newQObject(this);
    //  mScriptEngine->globalObject().setProperty("gv", gv);
    //
    //  QScriptValue draw2dHandle = mScriptEngine->newQObject(draw2d);
    //  mScriptEngine->globalObject().setProperty("view2d", draw2dHandle);
    //
    //  lct_view->addObjectsToScriptEngine(mScriptEngine);
    //  geo_view->addObjectsToScriptEngine(mScriptEngine);
    //  drw_view->addObjectsToScriptEngine(mScriptEngine);
    //  mObjectView->addObjectsToScriptEngine(mScriptEngine);
}

void GeodesicView::initStatusTips()
{
#ifdef SHOW_STATUS_TIPS
    cob_metric->setStatusTip(tr("Select a space-time metric."));
    cob_integrator->setStatusTip(tr("Select an integrator."));
    chb_stepsize_controlled->setStatusTip(tr("Use step size control for integration."));
    led_maxpoints->setStatusTip(tr("Maximum number of points."));
    led_stepsize->setStatusTip(tr("Initial step size."));
    led_constraint_eps->setStatusTip(tr("Epsilon domain for constraint equation."));
    led_eps_abs->setStatusTip(tr("Epsilon domain for absolute error."));
    led_eps_rel->setStatusTip(tr("Epsilon domain for relative error."));
    led_max_stepsize->setStatusTip(tr("Maximum step size."));
#endif
}

void GeodesicView::initGUI()
{
    // ---------------------------------
    //   drawing
    // ---------------------------------
    opengl = new OpenGL3dModel(&mParams);
    QFrame* wgt_opengl = new QFrame();
    QGridLayout* layout_opengl = new QGridLayout();
    layout_opengl->addWidget(opengl, 0, 0);
    wgt_opengl->setLayout(layout_opengl);

    draw2d = new OpenGL2dModel(&mParams);
    QFrame* wgt_2d = new QFrame();
    QGridLayout* layout_2d = new QGridLayout();
    layout_2d->addWidget(draw2d, 0, 0);
    wgt_2d->setLayout(layout_2d);

    tab_draw = new QTabWidget();
    tab_draw->addTab(wgt_opengl, tr("OpenGL 3D"));
    tab_draw->addTab(wgt_2d, tr("Draw 2D"));

    // ---------------------------------
    //    metric/integrator
    // ---------------------------------
    // add metrics to combobox
    cob_metric = new QComboBox();
    for (int i = 0; i < mObject.metricDB.getNumMetrics(); i++) {
        cob_metric->addItem(QString(mObject.metricDB.getMetricName(m4d::MetricList::enum_metric(i))));
    }

    tbw_metric_params = new QTableWidget(1, 3);
    QStringList tbw_metric_headers = QStringList() << "Name"
                                                   << "Value"
                                                   << "Step";
    tbw_metric_params->setHorizontalHeaderLabels(tbw_metric_headers);
    tbw_metric_params->setAlternatingRowColors(true);
    QHeaderView* header = tbw_metric_params->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    tbw_metric_params->setMaximumHeight(150);
    wgt_metric = new QWidget();

    QGridLayout* layout_metric = new QGridLayout();
    layout_metric->addWidget(cob_metric, 0, 0);
    layout_metric->addWidget(tbw_metric_params, 1, 0);
    wgt_metric->setLayout(layout_metric);

    // add integrators to combobox
    cob_integrator = new QComboBox();
    for (unsigned int i = 0; i < m4d::NUM_GEOD_SOLVERS; i++) {
        cob_integrator->addItem(QString(m4d::stl_solver_names[i]));
    }

    chb_stepsize_controlled = new QCheckBox("stepsize controlled");
    chb_stepsize_controlled->setCheckState(Qt::Unchecked);
    chb_stepsize_controlled->setEnabled(false);

    chb_drawstyle = new QCheckBox("draw with lines");
    chb_drawstyle->setCheckState(Qt::Checked);

    QLabel* lab_maxpoints = new QLabel("max points");
    led_maxpoints = new QLineEdit(QString::number(DEF_MAX_NUM_POINTS));
    led_maxpoints->setValidator(new QIntValidator(led_maxpoints));

    QLabel* lab_stepsize = new QLabel("step size");
    led_stepsize = new QLineEdit(QString::number(mObject.stepsize));
    led_stepsize->setValidator(new QDoubleValidator(led_stepsize));

    QLabel* lab_max_stepsize = new QLabel("max step");
    led_max_stepsize = new QLineEdit(QString::number(mObject.max_stepsize));
    led_max_stepsize->setValidator(new QDoubleValidator(led_max_stepsize));
    led_max_stepsize->setEnabled(false);

    // QLabel* lab_min_stepsize = new QLabel("min step");
    led_min_stepsize = new QLineEdit(QString::number(mObject.min_stepsize));
    led_min_stepsize->setValidator(new QDoubleValidator(led_min_stepsize));
    led_min_stepsize->setEnabled(false);

    QLabel* lab_eps_abs = new QLabel("eps abs");
    led_eps_abs = new QLineEdit(QString::number(mObject.epsAbs));
    led_eps_abs->setValidator(new QDoubleValidator(led_eps_abs));
    led_eps_abs->setEnabled(false);

    QLabel* lab_eps_rel = new QLabel("eps rel");
    led_eps_rel = new QLineEdit(QString::number(mObject.epsRel));
    led_eps_rel->setValidator(new QDoubleValidator(led_eps_rel));
    led_eps_rel->setEnabled(false);

    QLabel* lab_constraint_eps = new QLabel("constr. eps");
    led_constraint_eps = new QLineEdit(QString::number(DEF_CONSTRAINT_EPSILON));
    led_constraint_eps->setValidator(new QDoubleValidator(led_constraint_eps));

    // QLabel* lab_resize_eps = new QLabel("resize eps");
    led_resize_eps = new QLineEdit(QString::number(DEF_RESIZE_EPSILON));
    led_resize_eps->setValidator(new QDoubleValidator(led_resize_eps));

    // QLabel* lab_resize_fac = new QLabel("resize fac");
    led_resize_fac = new QLineEdit(QString::number(DEF_RESIZE_FACTOR));
    led_resize_fac->setValidator(new QDoubleValidator(led_resize_fac));
    led_resize_fac->setEnabled(false);

    wgt_integrator = new QWidget();

    QGridLayout* layout_integrator = new QGridLayout();
    layout_integrator->addWidget(cob_integrator, 0, 0, 1, 4);
    layout_integrator->addWidget(chb_stepsize_controlled, 1, 2, 1, 2);
    layout_integrator->addWidget(chb_drawstyle, 1, 0, 1, 2);
    layout_integrator->addWidget(lab_maxpoints, 2, 0);
    layout_integrator->addWidget(led_maxpoints, 2, 1);
    layout_integrator->addWidget(lab_stepsize, 3, 0);
    layout_integrator->addWidget(led_stepsize, 3, 1);
    layout_integrator->addWidget(lab_max_stepsize, 4, 2);
    layout_integrator->addWidget(led_max_stepsize, 4, 3);
    //  layout_integrator -> addWidget( lab_min_stepsize, 4, 2 );
    //  layout_integrator -> addWidget( led_min_stepsize, 4, 3 );

    layout_integrator->addWidget(lab_eps_abs, 2, 2);
    layout_integrator->addWidget(led_eps_abs, 2, 3);
    layout_integrator->addWidget(lab_eps_rel, 3, 2);
    layout_integrator->addWidget(led_eps_rel, 3, 3);
    layout_integrator->addWidget(lab_constraint_eps, 4, 0);
    layout_integrator->addWidget(led_constraint_eps, 4, 1);
    /*
    layout_integrator -> addWidget( lab_resize_eps, 6, 0 );
    layout_integrator -> addWidget( led_resize_eps, 6, 1 );
    layout_integrator -> addWidget( lab_resize_fac, 6, 2 );
    layout_integrator -> addWidget( led_resize_fac, 6, 3 );
    */
    layout_integrator->setRowStretch(5, 10);
    wgt_integrator->setLayout(layout_integrator);

    QLabel* lab_speed_of_light = new QLabel("SpeedOfLight");
    led_speed_of_light = new QLineEdit(QString::number(1.0));
    led_speed_of_light->setValidator(new QDoubleValidator(led_speed_of_light));

    QLabel* lab_grav_constant = new QLabel("GravConstant");
    led_grav_constant = new QLineEdit(QString::number(1.0));
    led_grav_constant->setValidator(new QDoubleValidator(led_grav_constant));

    QLabel* lab_dielectric_perm = new QLabel("DielectricPerm");
    led_dielectric_perm = new QLineEdit(QString::number(1.0));
    led_dielectric_perm->setValidator(new QDoubleValidator(led_dielectric_perm));

    pub_si_units = new QPushButton("SI units");
    pub_geom_units = new QPushButton("Geom units");
    wgt_constants = new QWidget();

    QHBoxLayout* layout_units = new QHBoxLayout();
    layout_units->addWidget(pub_si_units);
    layout_units->addWidget(pub_geom_units);

    QGridLayout* layout_constants = new QGridLayout();
    layout_constants->addWidget(lab_speed_of_light, 0, 0);
    layout_constants->addWidget(led_speed_of_light, 0, 1);
    layout_constants->addWidget(lab_grav_constant, 1, 0);
    layout_constants->addWidget(led_grav_constant, 1, 1);
    layout_constants->addWidget(lab_dielectric_perm, 2, 0);
    layout_constants->addWidget(led_dielectric_perm, 2, 1);
    layout_constants->addLayout(layout_units, 3, 0, 1, 2);
    layout_constants->setRowStretch(4, 10);
    wgt_constants->setLayout(layout_constants);

    tab_metricInt = new QTabWidget;
    tab_metricInt->addTab(wgt_metric, tr("Metric"));
    tab_metricInt->addTab(wgt_integrator, tr("Integrator"));
    // tab_metricInt->addTab(wgt_constants, tr("Constants"));

    QGroupBox* grb_metricInt = new QGroupBox("Metric / Integrator");
    QVBoxLayout* layout_metricInt = new QVBoxLayout();
    layout_metricInt->addWidget(tab_metricInt);
    layout_metricInt->addStretch(10);
    grb_metricInt->setLayout(layout_metricInt);

    // ---------------------------
    //  local tetrad view
    // ---------------------------
    lct_view = new LoctedView();

    // ---------------------------------
    //    geodesic
    // ---------------------------------
    mOglJacobi = new OpenGLJacobiModel(&mParams);
    geo_view = new GeodView(&mParams, opengl, mOglJacobi);
#ifdef WIN32 // Warum das hier nötig unter Windows ist weiß ich nicht so recht, aber so wird das CompassDial richtig
             // dargestellt.
    geo_view->setMinimumWidth(530);
#endif

    // ---------------------------------
    //    draw handling
    // ---------------------------------
    drw_view = new DrawView(opengl, draw2d, mOglJacobi, &mParams);

    mObjectView = new ObjectView;
    mReportText = new ReportDialog();
    mProtDialog = new ProtDialog;

    // ---------------------------------
    //  parameter frame
    // ---------------------------------
    QScrollArea* scr_area = new QScrollArea();
    scr_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scr_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scr_area->setWidgetResizable(true);

    QFrame* frame = new QFrame();
    // frame->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* layout_frame = new QVBoxLayout();
    layout_frame->addWidget(grb_metricInt);
    layout_frame->addWidget(lct_view);
    layout_frame->addWidget(geo_view);
    layout_frame->addWidget(drw_view);
    frame->setLayout(layout_frame);
    scr_area->setWidget(frame);

    // ---------------------------------
    //  layout
    // ---------------------------------
    QDockWidget* dw = new QDockWidget();
    dw->setWidget(scr_area);
    dw->setFeatures(
        QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    dw->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    dw->setWindowTitle("Parameters");

    setCentralWidget(tab_draw);
    addDockWidget(Qt::RightDockWidgetArea, dw);

    // ---------------------------
    //    status bar
    // ---------------------------
#ifdef SHOW_CALC_TIME
    lab_calc_time = new QLabel("calc time");
    lcd_calc_time = new QLCDNumber();
    lcd_calc_time->setSegmentStyle(QLCDNumber::Filled);
    lcd_calc_time->display(0);
#endif

    QLabel* lab_num_points = new QLabel("# points");
    lcd_num_points = new QLCDNumber();
    // lcd_num_points->setDigitCount(6);
    lcd_num_points->setSegmentStyle(QLCDNumber::Filled);
    lcd_num_points->display(0);

    led_status = new QLineEdit();
    led_status->setEnabled(false);
    // led_status->setMaximumWidth(400);

    mStatus = new QStatusBar();
#ifdef SHOW_CALC_TIME
    mStatus->addPermanentWidget(lab_calc_time);
    mStatus->addPermanentWidget(lcd_calc_time);
#endif
    mStatus->addPermanentWidget(lab_num_points);
    mStatus->addPermanentWidget(lcd_num_points);
    mStatus->addPermanentWidget(led_status);

    setStatusBar(mStatus);
}

void GeodesicView::initControl()
{
    connect(tab_draw, SIGNAL(currentChanged(int)), this, SLOT(slot_calcGeodesic()));

    connect(cob_metric, SIGNAL(activated(int)), this, SLOT(slot_setCurrentMetric()));
    connect(cob_integrator, SIGNAL(activated(int)), this, SLOT(slot_setGeodSolver()));

    connect(led_speed_of_light, SIGNAL(editingFinished()), this, SLOT(slot_setUnits()));
    connect(led_grav_constant, SIGNAL(editingFinished()), this, SLOT(slot_setUnits()));
    connect(led_dielectric_perm, SIGNAL(editingFinished()), this, SLOT(slot_setUnits()));

    connect(pub_si_units, SIGNAL(pressed()), this, SLOT(slot_setSIunits()));
    connect(pub_geom_units, SIGNAL(pressed()), this, SLOT(slot_setGeomUnits()));
    // connect( pub_si_units,   SIGNAL( pressed() ), drw_view, SLOT( slot_embParamChanged() ) );
    // connect( pub_geom_units, SIGNAL( pressed() ), drw_view, SLOT( slot_embParamChanged() ) );

    connect(led_maxpoints, SIGNAL(editingFinished()), this, SLOT(slot_setMaxNumPoints()));
    connect(chb_stepsize_controlled, SIGNAL(stateChanged(int)), this, SLOT(slot_setStepsizeControl()));
    connect(led_stepsize, SIGNAL(editingFinished()), this, SLOT(slot_setIntegratorParams()));
    connect(led_eps_abs, SIGNAL(editingFinished()), this, SLOT(slot_setIntegratorParams()));
    connect(led_eps_rel, SIGNAL(editingFinished()), this, SLOT(slot_setIntegratorParams()));
    connect(led_constraint_eps, SIGNAL(editingFinished()), this, SLOT(slot_setConstraintEps()));
    connect(led_resize_eps, SIGNAL(editingFinished()), this, SLOT(slot_setResize()));
    connect(led_resize_fac, SIGNAL(editingFinished()), this, SLOT(slot_setResize()));
    connect(chb_drawstyle, SIGNAL(stateChanged(int)), this, SLOT(slot_setDrawWithLines()));
    connect(led_max_stepsize, SIGNAL(editingFinished()), this, SLOT(slot_setMaxStepSize()));
    connect(led_min_stepsize, SIGNAL(editingFinished()), this, SLOT(slot_setMinStepSize()));

    connect(lct_view, SIGNAL(calcGeodesic()), this, SLOT(slot_calcGeodesic()));
    connect(geo_view, SIGNAL(calcGeodesic()), this, SLOT(slot_calcGeodesic()));
    connect(geo_view, SIGNAL(changeGeodType()), drw_view, SLOT(slot_adjustAPname()));
    connect(drw_view, SIGNAL(calcGeodesic()), this, SLOT(slot_calcGeodesic()));
    connect(drw_view, SIGNAL(colorChanged()), this, SLOT(slot_setOpenGLcolors()));
    connect(drw_view, SIGNAL(lastPointChanged(int)), geo_view, SLOT(slot_showLastJacobi(int)));

    connect(geo_view, SIGNAL(changeGeodType()), mOglJacobi, SLOT(update()));

    connect(mObjectView, SIGNAL(objectsChanged()), this, SLOT(slot_objChanged()));
    connect(mProtDialog, SIGNAL(emitWriteProt()), this, SLOT(slot_write_prot()));
}

bool GeodesicView::setMetric(m4d::MetricList::enum_metric metric)
{
    mObject.currMetric = mObject.metricDB.getMetric(metric);

    if (mObject.currMetric == nullptr) {
        return false;
    }
    mObject.currMetric->printF();

    return true;
}

bool GeodesicView::setMetricNamesAndCoords()
{
    for (int i = tbw_metric_params->rowCount(); i >= 0; --i) {
        tbw_metric_params->removeRow(0);
    }

    if (mObject.currMetric == nullptr) {
        return false;
    }

    /* --- set parameter names --- */
    std::vector<std::string> paramNames;
    mObject.currMetric->getParamNames(paramNames);

    for (size_t i = 0; i < paramNames.size(); i++) {
        tbw_metric_params->insertRow(static_cast<int>(i));
        QTableWidgetItem* item_name = new QTableWidgetItem();
        item_name->setText(paramNames[i].c_str());
        item_name->setFlags(Qt::ItemIsEnabled);
        tbw_metric_params->setItem(static_cast<int>(i), 0, item_name);

        double value;
        mObject.currMetric->getParam(paramNames[i].c_str(), value);
        DoubleEdit* led_value = new DoubleEdit(DEF_PREC_METRIC_PAR, value, 0.01);
        led_value->setObjectName(QString("%1.%2").arg(paramNames[i].c_str(), "value"));

        DoubleEdit* led_step = new DoubleEdit(DEF_PREC_METRIC_PAR, 0.01, 0.001);
        led_step->setObjectName(QString("%1.%2").arg(paramNames[i].c_str(), "step"));

        tbw_metric_params->setCellWidget(static_cast<int>(i), 1, led_value);
        tbw_metric_params->setCellWidget(static_cast<int>(i), 2, led_step);

        connect(led_value, SIGNAL(editingFinished()), this, SLOT(slot_metricParamChanged()));
        connect(led_step, SIGNAL(editingFinished()), led_value, SLOT(slot_setStep()));
    }
    // tbw_metric_params->resizeColumnsToContents();

    /* --- set coordinate names --- */
    drw_view->adjustCoordNames();
    lct_view->adjustCoordNames();

    /* --- set embedding to mParams --- */
    if (!mParams.opengl_emb_params.empty()) {
        mParams.opengl_emb_params.clear();
    }

    mObject.currMetric->getEmbeddingMap(mParams.opengl_emb_params);
    return true;
}

bool GeodesicView::setGeodSolver(m4d::enum_integrator type)
{
    if (mObject.currMetric == nullptr) {
        return false;
    }

    if (mObject.geodSolver != nullptr) {
        delete mObject.geodSolver;
    }
    mObject.geodSolverType = type;

    const gsl_odeiv_step_type* step_type = gsl_odeiv_step_rk4;
    switch (mObject.geodSolverType) {
        case m4d::gsUnknown:
        case m4d::gsIrk4:
#ifdef USE_DP_INT
        case m4d::gsIdp54:
        case m4d::gsIdp65:
#endif
        case m4d::gsInrbs:
            break;
        case m4d::gsIgslrk2:
            step_type = gsl_odeiv_step_rk2;
            break;
        case m4d::gsIgslrk4:
            step_type = gsl_odeiv_step_rk4;
            break;
        case m4d::gsIgslfehlberg:
            step_type = gsl_odeiv_step_rkf45;
            break;
        case m4d::gsIgslcash:
            step_type = gsl_odeiv_step_rkck;
            break;
        case m4d::gsIgslprinc:
            step_type = gsl_odeiv_step_rk8pd;
            break;
        case m4d::gsIi2:
            step_type = gsl_odeiv_step_rk2imp;
            break;
        /*
            case m4d::gsIbs:
            step_type = gsl_odeiv_step_bsimp;
            break;
            */
        case m4d::gsIm1:
            step_type = gsl_odeiv_step_gear1;
            break;
        case m4d::gsIm2:
            step_type = gsl_odeiv_step_gear2;
            break;
    }

    chb_stepsize_controlled->setEnabled(false);
    led_eps_abs->setEnabled(false);
    led_eps_rel->setEnabled(false);
    led_max_stepsize->setEnabled(false);

    if (type == m4d::gsIrk4) {
        mObject.geodSolver = new m4d::GeodesicRK4(mObject.currMetric, mObject.type);
    }
#ifdef USE_DP_INT
    else if (type == m4d::gsIdp54) {
        chb_stepsize_controlled->setEnabled(true);
        led_eps_abs->setEnabled(true);

        mObject.geodSolver = new m4d::GeodesicDP54(mObject.currMetric, mObject.type);
        mObject.geodSolver->setStepSizeControlled(chb_stepsize_controlled->isChecked());
        mObject.geodSolver->setEpsilons(led_eps_abs->text().toDouble(), led_eps_rel->text().toDouble());
        mObject.geodSolver->setAffineParamStep(led_stepsize->text().toDouble());
    }
    else if (type == m4d::gsIdp65) {
        chb_stepsize_controlled->setEnabled(true);
        led_eps_abs->setEnabled(true);

        mObject.geodSolver = new m4d::GeodesicDP65(mObject.currMetric, mObject.type);
        mObject.geodSolver->setStepSizeControlled(chb_stepsize_controlled->isChecked());
        mObject.geodSolver->setEpsilons(led_eps_abs->text().toDouble(), led_eps_rel->text().toDouble());
        mObject.geodSolver->setAffineParamStep(led_stepsize->text().toDouble());
    }
#endif
    else if (type == m4d::gsInrbs) {
        mObject.geodSolver = new m4d::GeodesicBS(mObject.currMetric, mObject.type);
        mObject.geodSolver->setStepSizeControlled(chb_stepsize_controlled->isChecked());
        mObject.geodSolver->setEpsilons(led_eps_abs->text().toDouble(), led_eps_rel->text().toDouble());
        mObject.geodSolver->setAffineParamStep(led_stepsize->text().toDouble());
        led_max_stepsize->setEnabled(true);
        mObject.geodSolver->setMaxAffineParamStep(led_max_stepsize->text().toDouble());
    }
    else {
        chb_stepsize_controlled->setEnabled(true);
        led_eps_abs->setEnabled(true);
        led_eps_rel->setEnabled(true);

        mObject.geodSolver = new m4d::GeodesicGSL(
            mObject.currMetric, step_type, static_cast<int>(mObject.geodSolverType), mObject.type);
        mObject.geodSolver->setStepSizeControlled(chb_stepsize_controlled->isChecked());
        mObject.geodSolver->setEpsilons(led_eps_abs->text().toDouble(), led_eps_rel->text().toDouble());
        mObject.geodSolver->setAffineParamStep(led_stepsize->text().toDouble());
        led_max_stepsize->setEnabled(true);
        mObject.geodSolver->setMaxAffineParamStep(led_max_stepsize->text().toDouble());
    }

    double dblmax = std::numeric_limits<double>::max();
    mObject.geodSolver->setBoundingBox(
        m4d::vec4(-dblmax, -dblmax, -dblmax, -dblmax), m4d::vec4(dblmax, dblmax, dblmax, dblmax));
    mObject.geodSolver->setGeodesicType(mObject.type);

    // mObject.geodSolver->print();

    led_constraint_eps->setText(QString::number(DEF_CONSTRAINT_EPSILON));
    return true;
}

void GeodesicView::setUnits(const double sol, const double grav, const double diperm)
{
    if (mObject.currMetric != nullptr) {
        mObject.currMetric->setUnits(sol, grav, diperm);
    }

    mObject.speed_of_light = sol;
    mObject.grav_constant = grav;
    mObject.dielectric_perm = diperm;
    calculateGeodesic();
}

void GeodesicView::calculateGeodesic()
{
    if ((mObject.currMetric == nullptr) || (mObject.geodSolver == nullptr)) {
        return;
    }

    double eta = 1.0;
    double y0 = 1.0;

    if (mObject.type == m4d::enum_geodesic_timelike && fabs(mObject.vel) < 1.0) {
        double beta = mObject.vel / mObject.currMetric->speed_of_light();
        double gm = 1.0 - beta * beta;
        if (gm > 0.0) {
            y0 = mObject.currMetric->speed_of_light() / sqrt(gm);
            eta = beta * y0;
        }
    }

    m4d::vec4 b[4];
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            b[i] += mObject.lorentz.getElem(i, k) * mObject.base[k];
        }
    }

    // Initial direction with respect to natural local tetrad:
    m4d::vec4 locDir = SIGNUM(mObject.timeDirection) * y0 * b[0]
        + eta * (mObject.startDir[0] * b[1] + mObject.startDir[1] * b[2] + mObject.startDir[2] * b[3]);
    m4d::vec3 nullDir = m4d::vec3(mObject.startDir[0], mObject.startDir[1], mObject.startDir[2]);
    m4d::vec4 coDir;
    mObject.currMetric->localToCoord(mObject.startPos, locDir, coDir, mObject.tetradType);
    mObject.geodSolver->setAffineParamStep(mObject.stepsize);
    mObject.coordDir = coDir;
#if 0
    mObject.geodSolver->print();
    mObject.startPos.print(std::cerr, "initPos: ", 15);
    mObject.startDir.print(std::cerr, "initDir: ", 15);
    coDir.print(std::cerr, "coDir: ", 15);
#endif

    m4d::vec3 locX = m4d::vec3(1.0, 0.0, 0.0);
    m4d::vec3 locY = m4d::vec3(0.0, 1.0, 0.0);
    m4d::vec3 locZ = m4d::vec3(0.0, 0.0, 1.0);
    m4d::vec3 lX, lY, lZ;

    switch (mParams.opengl_sachs_system) {
        case enum_sachs_e1e2e3:
            lX = locX;
            lY = locY;
            lZ = locZ;
            break;
        case enum_sachs_e2e3e1:
            lX = locY;
            lY = locZ;
            lZ = locX;
            break;
        case enum_sachs_e3e1e2:
            lX = locZ;
            lY = locX;
            lZ = locY;
            break;
    }

    m4d::enum_break_condition breakCond = m4d::enum_break_none;

    // integrate either the standard geodesic equation...
    if (mObject.type == m4d::enum_geodesic_lightlike || mObject.type == m4d::enum_geodesic_timelike) {
        breakCond = mObject.geodSolver->calculateGeodesic(
            mObject.startPos, coDir, mObject.maxNumPoints, mObject.points, mObject.dirs, mObject.lambda);
    }
    // ... or the geodesic equation together with the parallel transport of the Sachs basis and the Jacobi equation
    else if (mObject.type == m4d::enum_geodesic_lightlike_sachs) {
        breakCond = mObject.geodSolver->calcSachsJacobi(mObject.startPos, coDir, nullDir, lX, lY, lZ, b[0], b[1], b[2],
            b[3], mObject.tetradType, mObject.maxNumPoints, mObject.points, mObject.dirs, mObject.lambda,
            mObject.sachs1, mObject.sachs2, mObject.jacobi, mObject.maxJacobi);
    }
    // breakCond =
    // mObject.geodSolver->calcSachsJacobi(mObject.startPos,nullDir,coDir,mObject.base[0],mObject.base[1],mObject.base[2],mObject.base[3],mObject.maxNumPoints,mObject.points,mObject.dirs,mObject.lambda,mObject.sachs1,mObject.sachs2,mObject.jacobi);

    // ????? Welche der beiden oberen Methoden stimmt,  b[0]... oder mObject.base[0]... ??

    led_status->setText(m4d::stl_break_condition[breakCond]);

    lcd_num_points->display(int(mObject.points.size()));
    drw_view->setGeodLength(int(mObject.points.size()));

    if (tab_draw->currentIndex() == 1) { // set points for 2D visualization...
        int absIdx, ordIdx;
        drw_view->getDrawAbsOrdIndex(absIdx, ordIdx);
        draw2d->setPoints(mObject.currMetric->getCurrDrawType(drw_view->getDrawTypeName()));
    }
    else if (tab_draw->currentIndex() == 0) { // set sachs axes and points for 3D visualization...
        if (mObject.type == m4d::enum_geodesic_lightlike_sachs) {
            opengl->setSachsAxes(false);
        }
        else {
            opengl->clearSachsAxes();
        }
        opengl->setPoints(mObject.currMetric->getCurrDrawType(drw_view->getDrawType3DName()));
    }
#if 0
    for (int i = 0; i < mObject.points.size(); i++) {
        fprintf(stdout, "%5d %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8e %12.8f\n", i, mObject.lambda[i], mObject.points[i].x(1), mObject.points[i].x(3), mObject.jacobi[i][0], mObject.jacobi[i][1], mObject.jacobi[i][2], mObject.jacobi[i][3], mObject.jacobi[i][4]);
    }
#endif
    if (mObject.maxNumPoints > 1) {
        mOglJacobi->showNumJacobi(mObject.maxNumPoints - 1);
        geo_view->slot_showLastJacobi(mObject.maxNumPoints - 1);
    }

    if (mReportText->isVisible()) {
        slot_show_report();
    }

#ifdef SHOW_CALC_TIME
    lcd_calc_time->display(mObject.geodSolver->getCalcTime());
#endif
}

void GeodesicView::calculateGeodesicData()
{
    if ((mObject.currMetric != nullptr) && (mObject.geodSolver != nullptr)) {
        double eta = 1.0;
        double y0 = 1.0;

        if (mObject.type == m4d::enum_geodesic_timelike && fabs(mObject.vel) < 1.0) {
            y0 = mObject.currMetric->speed_of_light() / sqrt(1.0 - mObject.vel * mObject.vel);
            eta = mObject.vel * y0;
        }

        m4d::vec4 b[4];
        for (int i = 0; i < 4; i++) {
            b[i] = mObject.lorentz * mObject.base[i];
        }

        m4d::vec4 locDir = SIGNUM(mObject.timeDirection) * y0 * b[0]
            + eta * (mObject.startDir[0] * b[1] + mObject.startDir[1] * b[2] + mObject.startDir[2] * b[3]);
        m4d::vec4 coDir;
        mObject.currMetric->localToCoord(mObject.startPos, locDir, coDir, mObject.tetradType);

        if (!mPointData.empty()) {
            mPointData.clear();
        }
        if (!mDirData.empty()) {
            mDirData.clear();
        }
        if (!mEpsData.empty()) {
            mEpsData.clear();
        }
        mObject.geodSolver->calculateGeodesicData(
            mObject.startPos, coDir, mObject.maxNumPoints, mPointData, mDirData, mEpsData, mObject.lambda);
    }
}

bool GeodesicView::saveSetting(QString filename)
{
    QDate dt = QDate::currentDate();
    if (!mObject.saveSettings(filename.toStdString().c_str(), dt.toString(Qt::LocaleDate).toStdString().c_str())) {
        led_status->setText("save settings failed");
        return false;
    }
    return true;
}

bool GeodesicView::saveObjects(QString filename)
{
    QFile outFile(filename);
    if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
        std::cerr << "Can't open file " << filename.toStdString() << " for output!" << std::endl;
        return false;
    }

    QTextStream out(&outFile);
    for (unsigned int i = 0; i < mObjects.size(); i++) {
        std::string line;
        mObjects[i]->printLine(line);
        out << QString(line.c_str()) << endl;
    }
    outFile.close();
    return true;
}

bool GeodesicView::saveReport(QString filename)
{
    char* text = nullptr;
    mObject.makeReport(text);

    QFile outFile(filename);
    if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
        std::cerr << "Can't open file " << filename.toStdString() << " for saving report!" << std::endl;
        return false;
    }

    QTextStream out(&outFile);
    out << text;
    outFile.close();

    SafeDelete<char>(text);
    return true;
}

void GeodesicView::setSetting()
{
    cob_metric->setCurrentIndex(cob_metric->findText(mObject.currMetric->getMetricName(), Qt::MatchExactly));
    setMetricNamesAndCoords();
    drw_view->resetAll();
    drw_view->adjustDrawTypes();
    drw_view->adjustEmbParams();

    opengl->clearEmbed();

    cob_integrator->setCurrentIndex(static_cast<int>(mObject.geodSolverType));
    setGeodSolver(mObject.geodSolverType);
    led_maxpoints->setText(QString::number(mObject.maxNumPoints));
    led_stepsize->setText(QString::number(mObject.stepsize));
    led_eps_abs->setText(QString::number(mObject.epsAbs));
    led_eps_rel->setText(QString::number(mObject.epsRel));
    led_constraint_eps->setText(QString::number(DEF_CONSTRAINT_EPSILON));
    mObject.geodSolver->setEpsilons(mObject.epsAbs, mObject.epsRel);
    mObject.geodSolver->setAffineParamStep(mObject.stepsize);

    lct_view->setBoostParams(mObject.boost_chi, mObject.boost_ksi, mObject.boost_beta);
    lct_view->adjustLocTedTypes();
    lct_view->updateData();
    geo_view->updateData();

    if (mObject.stepsizeControlled) {
        chb_stepsize_controlled->setCheckState(Qt::Checked);
    }
    else {
        chb_stepsize_controlled->setCheckState(Qt::Unchecked);
    }

    led_speed_of_light->setText(QString("%1").arg(mObject.speed_of_light, 0, 'f', 6));
    led_grav_constant->setText(QString::number(mObject.grav_constant));
    led_dielectric_perm->setText(QString::number(mObject.dielectric_perm));

    mObject.currMetric->setUnits(mObject.speed_of_light, mObject.grav_constant, mObject.dielectric_perm);
}

void GeodesicView::loadObjects(QString filename)
{
    if (!mObjects.empty()) {
        mObjects.clear();
    }

    opengl->clearAllObjects();
    draw2d->clearAllObjects();
    size_t num = static_cast<size_t>(readObjectFile(filename.toStdString(), mObjects));

    for (size_t i = 0; i < num; i++) {
        if (mObjects[i]->getObjectDim() == enum_object_dim_3d) {
            opengl->insertObject(mObjects[i]);
        }
        else if (mObjects[i]->getObjectDim() == enum_object_dim_2d) {
            draw2d->insertObject(mObjects[i]);
        }
    }
}

void GeodesicView::closeEvent(QCloseEvent* event)
{
    int button = QMessageBox::question(
        this, "Quit", "Really quit GeodesicViewer?", QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Ok);
    if (button == QMessageBox::Ok) {
        mObjectView->close();
        mProtDialog->close();
        mReportText->close();
        // mHelpDoc->close();
        event->accept();
    }
    else {
        event->ignore();
    }
}

#ifdef HAVE_NETWORK
void GeodesicView::slot_newConnect()
{
    mSocket = mServer->nextPendingConnection();
    mSocket->setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));
    connect(mSocket, SIGNAL(disconnected()), this, SLOT(slot_disconn()));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(slot_readData()));
    std::cerr << "new connection\n";
}

void GeodesicView::slot_disconn()
{
    // nothing
}

void GeodesicView::slot_readData()
{
    QByteArray block = mSocket->read(4096);
    QDataStream in(&block, QIODevice::ReadOnly);

    gvs_socket_data sdata;
    int bytesRead = in.readRawData((char*)&sdata, sizeof(gvs_socket_data));

    if (bytesRead == sizeof(gvs_socket_data)) {
        switch (sdata.task) {
            case enum_stask_initdir: {
                geo_view->setTimeDirection(static_cast<int>(sdata.vals[0]));
                geo_view->setDirection(sdata.vals[1], sdata.vals[2], sdata.vals[3]); // val, ksi, chi
                calculateGeodesic();
                break;
            }
            case enum_stask_initpos: {
                lct_view->setPosition(sdata.vals[0], sdata.vals[1], sdata.vals[2], sdata.vals[3]);
                break;
            }
        }
    }
}
#endif // HAVE_NETWORK

#ifdef HAVE_LUA
int laddGVObject(lua_State* L)
{
    GeodesicView* gv = GeodesicView::getInstance();

    if (lua_isnil(L, -1) || !lua_istable(L, -1)) {
        return 0;
    }

    MyObject* obj = new MyObject();

    int objType = enum_object_undefined;
    std::string sVal;
    if (getfield(L, "type", sVal)) {
        int t = 1;
        while (t < stl_object_type.size()) {
            if (sVal.compare(stl_object_type[t].toStdString()) == 0) {
                objType = t;
                break;
            }
            t++;
        }
    }

    double num_points = 100.0, width = 1.0, slices = 40, stacks = 40, fans = 40;
    getfield(L, "numPoints", num_points);
    getfield(L, "width", width);
    getfield(L, "slices", slices);
    getfield(L, "stacks", stacks);
    getfield(L, "fans", fans);

    std::vector<double> center, base, top, color;
    getfield(L, "pos", center);
    getfield(L, "base", base);
    getfield(L, "top", top);
    getfield(L, "color", color);

    std::vector<double> v1, v2, v3, v4, e1, e2, e3;
    getfield(L, "v1", v1);
    getfield(L, "v2", v2);
    getfield(L, "v3", v3);
    getfield(L, "v4", v4);
    getfield(L, "e1", e1);
    getfield(L, "e2", e2);
    getfield(L, "e3", e3);

    double iRadius = 0.0, oRadius = 1.0, radius = 1.0, sizeX = 1.0, sizeY = 1.0, sizeZ = 1.0;
    double bRadius = 1.0, tRadius = 1.0;

    getfield(L, "iRadius", iRadius);
    getfield(L, "oRadius", oRadius);
    getfield(L, "radius", radius);
    getfield(L, "bRadius", bRadius);
    getfield(L, "tRadius", tRadius);
    getfield(L, "sizeX", sizeX);
    getfield(L, "sizeY", sizeY);
    getfield(L, "sizeZ", sizeZ);

    switch (objType) {
        case enum_object_undefined: {
            break;
        }
        case enum_object_sphere2d: {
            if (center.size() > 1) {
                obj->create_2d_sphere(
                    (float)center[0], (float)center[1], (float)radius, (float)num_points, (float)width);
            }
            break;
        }
        case enum_object_sphere3d: {
            if (center.size() > 2) {
                obj->create_3d_sphere(
                    (float)center[0], (float)center[1], (float)center[2], (float)radius, (float)slices, (float)stacks);
            }
            break;
        }
        case enum_object_box2d: {
            if (center.size() > 1) {
                obj->create_2d_box((float)center[0], (float)center[1], (float)sizeX, (float)sizeY, (float)width);
            }
            break;
        }
        case enum_object_box3d: {
            if (center.size() > 2) {
                obj->create_3d_box((float)center[0], (float)center[1], (float)center[2], (float)sizeX, (float)sizeY,
                    (float)sizeZ, (float)width);
            }
            break;
        }
        case enum_object_line2d: {
            if (v1.size() > 1 && v2.size() > 1) {
                obj->create_2d_line((float)v1[0], (float)v1[1], (float)v2[0], (float)v2[1], (float)width);
            }
            break;
        }
        case enum_object_line3d: {
            if (v1.size() > 2 && v2.size() > 2) {
                obj->create_3d_line(
                    (float)v1[0], (float)v1[1], (float)v1[2], (float)v2[0], (float)v2[1], (float)v2[2], (float)width);
            }
            break;
        }
        case enum_object_quad2d: {
            if (v1.size() > 1 && v2.size() > 1 && v3.size() > 1 && v4.size() > 1) {
                obj->create_2d_quad((float)v1[0], (float)v1[1], (float)v2[0], (float)v2[1], (float)v3[0], (float)v3[1],
                    (float)v4[0], (float)v4[1], (float)width);
            }
            break;
        }
        case enum_object_plane3d: {
            // obj->create_3d_plane();
            break;
        }
        case enum_object_disk2d: {
            if (center.size() > 1) {
                obj->create_2d_disk((float)center[0], (float)center[1], (float)radius, (float)fans);
            }
            break;
        }
        case enum_object_disk3d: {
            // if (base.size() > 2 && top.size() > 2) {
            // obj->create_3d_disk((float)center[0],(float)center[1],(float)center[2],);
            //}
            break;
        }
        case enum_object_cylinder3d: {
            if (base.size() > 2 && top.size() > 2) {
                obj->create_3d_cylinder((float)base[0], (float)base[1], (float)base[2], (float)top[0], (float)top[2],
                    (float)top[3], (float)bRadius, (float)tRadius, (float)slices, (float(stacks)));
            }
            break;
        }
        case enum_object_torus3d: {
            // obj->create_3d_torus();
            break;
        }
        case enum_object_tube3d: {
            // obj->create_3d_tube();
            break;
        }
        case enum_object_text2d: {
            // obj->create_2d_text();
            break;
        }
        case enum_object_text3d: {
            // obj->create_3d_text();
            break;
        }
    }

    if (color.size() > 2) {
        obj->setColor((float)color[0], (float)color[1], (float)color[2]);
    }
    gv->addObject(obj);

    return 0;
}

int lclearAllGVObjects(lua_State*)
{
    GeodesicView* gv = GeodesicView::getInstance();
    gv->clearAllObjects();
    return 0;
}

int setGVCamera(lua_State* L)
{
    if (lua_isnil(L, -1) || !lua_istable(L, -1)) {
        return 0;
    }

    GeodesicView* gv = GeodesicView::getInstance();

    std::vector<double> eyePos;
    if (getfield(L, "pos", eyePos)) {
        if (eyePos.size() >= 3) {
            m4d::vec3 pos = m4d::vec3(eyePos[0], eyePos[1], eyePos[2]);
            gv->opengl->setCameraPos(pos);
        }
    }

    std::vector<double> viewDir;
    if (getfield(L, "dir", viewDir)) {
        if (viewDir.size() >= 3) {
            m4d::vec3 vdir = m4d::vec3(viewDir[0], viewDir[1], viewDir[2]);
            // gv->opengl->setC
        }
    }

    std::vector<double> poi;
    if (getfield(L, "poi", poi)) {
        if (poi.size() >= 3) {
            m4d::vec3 p = m4d::vec3(poi[0], poi[1], poi[2]);
            gv->opengl->setCameraPoi(p);
        }
    }

    std::vector<double> vup;
    if (getfield(L, "vup", vup)) {
        if (vup.size() >= 3) {
            m4d::vec3 u = m4d::vec3(vup[0], vup[1], vup[2]);
            u.print();
            gv->opengl->setCameraVup(u);
        }
    }

    std::vector<double> spherePos;
    if (getfield(L, "spos", spherePos)) {
        if (spherePos.size() >= 3) {
            double theta = spherePos[0] * DEG_TO_RAD;
            double phi = spherePos[1] * DEG_TO_RAD;
            double dist = spherePos[2];
            gv->opengl->setCameraSphere(theta, phi, dist);
            gv->opengl->updateGL();
        }
    }

    return 0;
}

#endif // HAVE_LUA
