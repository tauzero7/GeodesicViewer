######################################################################  PROJECT NAME
TARGET       = GeodesicViewer
PROJECT_MAIN = gviewer_main.cpp


######################################################################  RESSOURCES
RESOURCES     = gviewer.qrc

######################################################################  RELATIVE PATHS
VIEW_DIR     = $$TOP_DIR/view
MODEL_DIR    = $$TOP_DIR/model
UTILS_DIR    = $$TOP_DIR/utils

######################################################################  HEADERS and SOURCES

VIEW_HEADERS  =  $$VIEW_DIR/geodesic_view.h    $$VIEW_DIR/compass_dial.h  $$VIEW_DIR/dialwdg.h \
                 $$VIEW_DIR/locted_view.h      $$VIEW_DIR/geod_view.h \
                 $$VIEW_DIR/draw_view.h        $$VIEW_DIR/object_view.h \
                 $$VIEW_DIR/prot_view.h        $$VIEW_DIR/ledpub_widget.h \
                 $$VIEW_DIR/doc_view.h         $$VIEW_DIR/report_view.h

VIEW_SOURCES  =  $$VIEW_DIR/geodesic_view.cpp  $$VIEW_DIR/compass_dial.cpp $$VIEW_DIR/dialwdg.cpp \
                 $$VIEW_DIR/locted_view.cpp    $$VIEW_DIR/geod_view.cpp \
                 $$VIEW_DIR/draw_view.cpp      $$VIEW_DIR/object_view.cpp \
                 $$VIEW_DIR/prot_view.cpp      $$VIEW_DIR/ledpub_widget.cpp \
                 $$VIEW_DIR/doc_view.cpp       $$VIEW_DIR/report_view.cpp

MODEL_HEADERS =  $$MODEL_DIR/opengl3d_model.h    $$MODEL_DIR/openglJacobi_model.h    $$MODEL_DIR/opengl2d_model.h 
MODEL_SOURCES =  $$MODEL_DIR/opengl3d_model.cpp  $$MODEL_DIR/openglJacobi_model.cpp  $$MODEL_DIR/opengl2d_model.cpp 

UTILS_HEADERS =  $$UTILS_DIR/camera.h   $$UTILS_DIR/quaternions.h \
                 $$UTILS_DIR/doubleedit_util.h   $$UTILS_DIR/greek.h \
                 $$UTILS_DIR/myobject.h  $$UTILS_DIR/utilities.h \
                 $$UTILS_DIR/gramschmidt.h

UTILS_SOURCES =  $$UTILS_DIR/camera.cpp $$UTILS_DIR/quaternions.cpp \
                 $$UTILS_DIR/doubleedit_util.cpp $$UTILS_DIR/greek.cpp \
                 $$UTILS_DIR/myobject.cpp  $$UTILS_DIR/utilities.cpp \
		 $$UTILS_DIR/gramschmidt.cpp

GRAVLENS_HEADERS = $$GRAVLENS_DIR/magn_grid.h

GRAVLENS_SOURCES = $$GRAVLENS_DIR/magn_grid.cpp

######################################################################  COMPILER
QMAKE_CC        = g++
QMAKE_CXX       = g++
QMAKE_LINK      = g++

######################################################################  INCLUDE and DEPEND
INCLUDEPATH +=  . .. $$VIEW_DIR  $$MODEL_DIR $$UTILS_DIR \
                $$MOTION4D_DIR/include \
                $$GSL_DIR/include

win32:INCLUDEPATH +=$$MOTION4D_DIR/src

######################################################################  intermediate moc and object files
MOC_DIR      = $$TOP_DIR/compiled/moc
OBJECTS_DIR  = $$TOP_DIR/compiled/object


######################################################################  feste Angaben
CONFIG  += console warn_on
QT      += opengl webkit script
TEMPLATE = app

######################################################################  Input
HEADERS += $$VIEW_HEADERS  $$MODEL_HEADERS  $$UTILS_HEADERS  gdefs.h
SOURCES += $$VIEW_SOURCES  $$MODEL_SOURCES  $$UTILS_SOURCES  $$PROJECT_MAIN

######################################################################  destination dir
DESTDIR  = .


unix:LIBS +=  -Wl,-rpath $$M4D_LIB_DIR  -Wl,-rpath $$GSL_LIB_DIR \
             # -L$$M4D_LIB_DIR -lMotion4Dextra -lMotion4Dmath -lMotion4Dmetric -lMotion4Dmotion -Wl,-rpath $$M4D_LIB_DIR\
              -L$$M4D_LIB_DIR -lM4D \
              -L$$GSL_LIB_DIR -lgsl -lgslcblas \
              -L/usr/lib -lGL -lGLU

win32:LIBS += -mwindows \
              -L"$$GSL_LIB_DIR"  $$GSL_LIB_DIR/libgsl.a  $$GSL_LIB_DIR/libgslcblas.a \
              -L"$$MOTION4D_DIR/release" $$MOTION4D_DIR/release/$$M4D_LIB_NAME
