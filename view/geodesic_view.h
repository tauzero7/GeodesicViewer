/**
 * @file    geodesic_view.h
 * @author  Thomas Mueller
 *
 * @brief  Main GUI of the GeodesicViewer.
 *
 * This file is part of GeodesicView.
 */
#ifndef GEODESIC_VIEW_H
#define GEODESIC_VIEW_H

#include <cassert>
#include <iostream>

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>

#ifdef HAVE_NETWORK
#include <QTcpServer>
#include <QTcpSocket>
#endif // HAVE_NETWORK

#include <gdefs.h>

#include <opengl2d_model.h>
#include <opengl3d_model.h>

#include <doubleedit_util.h>
#include <greek.h>

#include <draw_view.h>
#include <geod_view.h>
#include <locted_view.h>
#include <object_view.h>
#include <prot_view.h>
//#include <doc_view.h>
#include <report_view.h>

#include <extra/m4dObject.h>
#include <metric/m4dMetricDatabase.h>
#include <motion/m4dMotionList.h>
#include <utils/myobject.h>
#include <utils/utilities.h>

#ifdef HAVE_LUA
#include "lua/m4dlua.h"
#endif

/**
 * @brief The GeodesicView class
 */
class GeodesicView : public QMainWindow
{
    Q_OBJECT

public:
    static GeodesicView* getInstance()
    {
        if (!m_instance) {
            m_instance = new GeodesicView();
        }
        return m_instance;
    }

    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

public slots:
    void slot_load_setting();
    void slot_save_setting();
    void slot_load_vparams();
    void slot_save_vparams();
    void slot_reset_vparams();

    void slot_save_image_2d();
    void slot_save_image_3d();

    void slot_load_all();

    void slot_prot_dialog();
    void slot_write_prot();
    void slot_quit();

#ifdef HAVE_LUA
    void slot_run_luaScript();
#endif

    void slot_reset();
    void slot_doc();
    void slot_about();

    void slot_changeDrawActive();

    void slot_animate();

    void slot_load_objects();
    void slot_append_objects();
    void slot_save_objects();
    void slot_show_objects();
    void slot_objChanged();

    void slot_show_report();
    void slot_save_report();

    void slot_setCurrentMetric();
    void slot_metricParamChanged();
    void slot_setGeodSolver();

    void slot_setMaxNumPoints();
    void slot_setStepsizeControl();
    void slot_setIntegratorParams();
    void slot_setDrawWithLines();

    void slot_setConstraintEps();
    void slot_setResize();

    void slot_setMaxStepSize();
    void slot_setMinStepSize();

    void slot_setSIunits();
    void slot_setGeomUnits();
    void slot_setUnits();

    void slot_calcGeodesic();

    void slot_setOpenGLcolors();

    void slot_executeScript();

    // script functions
    void reset();
    void setMetric(QString name);
    void setMetricParam(int num, double val);
    void setMetricParam(QString name, double val);
    void setSolver(QString name);
    void setSolverParam(QString name, double val);
    void wait(double sec);

    void addObject(MyObject* obj);
    void clearAllObjects();

protected:
    void init();
    void initGUI();
    void initControl();
    void initActions();
    void initMenus();
    void initScripting();
    void initStatusTips();

    void calculateGeodesic();
    void calculateGeodesicData();

    bool setMetric(m4d::MetricList::enum_metric metric);
    bool setMetricNamesAndCoords();
    bool setGeodSolver(m4d::enum_integrator type);

    void setUnits(const double sol, const double grav, const double diperm);

    bool saveSetting(QString filename);
    bool saveObjects(QString filename);
    bool saveReport(QString filename);

    void setSetting();
    void loadObjects(QString filename);

    void execScript(QString filename);

protected slots:
    virtual void closeEvent(QCloseEvent* event);

#ifdef HAVE_NETWORK
    void slot_newConnect();
    void slot_disconn();
    void slot_readData();
#endif // HAVE_NETWORK

public:
    OpenGL3dModel* opengl;

private:
    GeodesicView();
    GeodesicView(const GeodesicView&);
    ~GeodesicView();

    static GeodesicView* m_instance;

    class CGuard
    {
    public:
        ~CGuard()
        {
            if (GeodesicView::m_instance != nullptr) {
                delete GeodesicView::m_instance;
                GeodesicView::m_instance = nullptr;
            }
        }
    };

    friend class CGuard;

    struct_params mParams;

    // Tab widget holding 2D and 3D rendering window
    QTabWidget* tab_draw;

    OpenGL2dModel* draw2d;
    QAction* mActionMake3Dactive;
    QAction* mActionMake2Dactive;

    // ---- metric/integrator ----
    QTabWidget* tab_metricInt;

    QComboBox* cob_metric;
    QTableWidget* tbw_metric_params;
    QWidget* wgt_metric;

    QComboBox* cob_integrator;
    QCheckBox* chb_stepsize_controlled;
    QCheckBox* chb_drawstyle;
    QLineEdit* led_maxpoints;
    QLineEdit* led_stepsize;
    QLineEdit* led_max_stepsize;
    QLineEdit* led_min_stepsize;
    QLineEdit* led_eps_abs;
    QLineEdit* led_eps_rel;
    QLineEdit* led_constraint_eps;
    QLineEdit* led_resize_eps;
    QLineEdit* led_resize_fac;
    QWidget* wgt_integrator;

    QLineEdit* led_speed_of_light;
    QLineEdit* led_grav_constant;
    QLineEdit* led_dielectric_perm;
    QPushButton* pub_si_units;
    QPushButton* pub_geom_units;
    QWidget* wgt_constants;

    // ---- local tetrad ----
    LoctedView* lct_view;

    // ---- geodesic ----
    GeodView* geo_view;

    // ---- draw handing ----
    DrawView* drw_view;
    OpenGLJacobiModel* mOglJacobi;

    // ---- File menu ----
    QMenu* mFileMenu;
    QAction* mActionReset;
    QAction* mActionLoadSetting;
    QAction* mActionSaveSetting;
    QAction* mActionLoadViewParams;
    QAction* mActionSaveViewParams;
    QAction* mActionResetViewParams;
    QAction* mActionLoadAll;
    QAction* mActionSaveImage2D;
    QAction* mActionSaveImage3D;
    QAction* mActionWriteProtocoll;
    QAction* mActionQuit;

#ifdef HAVE_LUA
    QAction* mActionRunLuaScript;
    lua_State* mLuaState;
#endif // HAVE_LUA

    // ---- Object menu ----
    QMenu* mObjectMenu;
    QAction* mActionLoadObjectFile;
    QAction* mActionAppendObjectFile;
    QAction* mActionSaveObjectFile;
    QAction* mActionShowObjects;

    /* ---- Report menu ---- */
    QMenu* mReportMenu;
    QAction* mActionShowReport;
    QAction* mActionSaveReport;

    /* ---- Help menu ---- */
    QMenu* mHelpMenu;
    QAction* mActionAbout;
    // QAction*        mActionDoc;
    // DocView*        mHelpDoc;

    QAction* mActionPlay;
    QAction* mActionStepKsiPlus;
    QAction* mActionStepKsiMinus;
    QAction* mActionStepChiPlus;
    QAction* mActionStepChiMinus;

    ObjectView* mObjectView;
    ReportDialog* mReportText;
    ProtDialog* mProtDialog;

    // ---- QtScript ----
    // QScriptEngine*  mScriptEngine;
    QAction* mLoadScript;

    // ---- status bar ----
    QStatusBar* mStatus;
    QLineEdit* led_status;
    QLCDNumber* lcd_num_points;

#ifdef SHOW_CALC_TIME
    QLabel* lab_calc_time;
    QLCDNumber* lcd_calc_time;
#endif

    std::vector<m4d::vec4> mPointData;
    std::vector<m4d::vec4> mDirData;
    std::vector<double> mEpsData;
    std::vector<MyObject*> mObjects;

    GreekLetter mGreekLetter;

#ifdef HAVE_NETWORK
    QTcpServer* mServer;
    QTcpSocket* mSocket;
#endif // HAVE_NETWORK
};

#ifdef HAVE_LUA
int laddGVObject(lua_State* L);
int lclearAllGVObjects(lua_State* L);
int setGVCamera(lua_State* L);
#endif // HAVE_LUA

#endif
