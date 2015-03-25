######################################################################  PROJECT NAME
PROJECT_MAIN = gviewer_main.cpp

######################################################################  RESSOURCES
RESOURCES     = gviewer.qrc

######################################################################  RELATIVE PATHS
VIEW_DIR     = $$TOP_DIR/view
MODEL_DIR    = $$TOP_DIR/model
UTILS_DIR    = $$TOP_DIR/utils

######################################################################  HEADERS and SOURCES

VIEW_HEADERS  =  $$VIEW_DIR/geodesic_view.h    $$VIEW_DIR/compass_dial.h    $$VIEW_DIR/dialwdg.h \
                 $$VIEW_DIR/locted_view.h      $$VIEW_DIR/geod_view.h \
                 $$VIEW_DIR/draw_view.h        $$VIEW_DIR/object_view.h \
                 $$VIEW_DIR/prot_view.h        $$VIEW_DIR/ledpub_widget.h \
                 $$VIEW_DIR/report_view.h  # $$VIEW_DIR/doc_view.h

VIEW_SOURCES  =  $$VIEW_DIR/geodesic_view.cpp  $$VIEW_DIR/compass_dial.cpp   $$VIEW_DIR/dialwdg.cpp \
                 $$VIEW_DIR/locted_view.cpp    $$VIEW_DIR/geod_view.cpp \
                 $$VIEW_DIR/draw_view.cpp      $$VIEW_DIR/object_view.cpp \
                 $$VIEW_DIR/prot_view.cpp      $$VIEW_DIR/ledpub_widget.cpp \
                 $$VIEW_DIR/report_view.cpp  # $$VIEW_DIR/doc_view.cpp

MODEL_HEADERS =  $$MODEL_DIR/opengl3d_model.h    $$MODEL_DIR/openglJacobi_model.h \
                 $$MODEL_DIR/opengl2d_model.h
MODEL_SOURCES =  $$MODEL_DIR/opengl3d_model.cpp  $$MODEL_DIR/openglJacobi_model.cpp \
                 $$MODEL_DIR/opengl2d_model.cpp

UTILS_HEADERS =  $$UTILS_DIR/camera.h   $$UTILS_DIR/quaternions.h \
                 $$UTILS_DIR/doubleedit_util.h   $$UTILS_DIR/greek.h \
                 $$UTILS_DIR/myobject.h  $$UTILS_DIR/utilities.h \
                 $$UTILS_DIR/gramschmidt.h

UTILS_SOURCES =  $$UTILS_DIR/camera.cpp $$UTILS_DIR/quaternions.cpp \
                 $$UTILS_DIR/doubleedit_util.cpp $$UTILS_DIR/greek.cpp \
                 $$UTILS_DIR/myobject.cpp  $$UTILS_DIR/utilities.cpp \
                 $$UTILS_DIR/gramschmidt.cpp

include($$M4D_DIR/m4d_sources.pri)

######################################################################  COMPILER
#QMAKE_CC        = g++
#QMAKE_CXX       = g++
#QMAKE_LINK      = g++

######################################################################  INCLUDE and DEPEND
INCLUDEPATH +=  . .. $$VIEW_DIR  $$MODEL_DIR $$UTILS_DIR $$M4D_SRC_DIR \
                $$GSL_DIR/include

DEPENDPATH  += 

CONFIG(debug, debug|release) {
    TARGET       = GeodesicViewerd
    DESTDIR      = $$TOP_DIR
    MOC_DIR      = $$TOP_DIR/compiled/debug/moc
    OBJECTS_DIR  = $$TOP_DIR/compiled/debug/object
    RCC_DIR      = $$TOP_DIR/compiled/debug
}

CONFIG(release, debug|release) {
    TARGET       = GeodesicViewer
    DESTDIR      = $$TOP_DIR
    MOC_DIR      = $$TOP_DIR/compiled/release/moc
    OBJECTS_DIR  = $$TOP_DIR/compiled/release/object
    RCC_DIR      = $$TOP_DIR/compiled/release
}


CONFIG  += console warn_on
QT      += core gui opengl script network
TEMPLATE = app

#contains(QT_VERSION,^4\\.[0-9]\\..*) {
#    message("Qt version $${QT_VERSION}.")
#    QT += webkit
#}
#
#contains(QT_VERSION,^5\\.[0-9]\\..*) {
#    message("Qt version $${QT_VERSION}.")
#    QT += webkitwidgets
#}


HEADERS += $$VIEW_HEADERS  $$MODEL_HEADERS  $$UTILS_HEADERS  $$M4D_HEADERS gdefs.h
SOURCES += $$VIEW_SOURCES  $$MODEL_SOURCES  $$UTILS_SOURCES  $$M4D_SOURCES $$PROJECT_MAIN


unix:!macx {
    LIBS +=  -Wl,-rpath $$GSL_LIB_DIR \
             -L$$GSL_LIB_DIR -lgsl -lgslcblas \
             -L/usr/lib -lGL -lGLU
}

win32 {
    DEFINES += METRIC_EXPORTS MOTION_EXPORTS MATH_EXPORTS EXTRA_EXPORTS
    LIBS +=   -L"$$GSL_LIB_DIR" -lgsl -lcblas
}

macx {
    LIBS +=  -Wl,-rpath $$GSL_LIB_DIR \
              -L$$GSL_LIB_DIR -lgsl -lgslcblas
}
