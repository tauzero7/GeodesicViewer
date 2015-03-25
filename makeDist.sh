#!/bin/bash

GVS_DIR=GeodesicViewer
M4D_DIR=libMotion4D
TODAY=$(date +'%Y%m%d')

cd ..
zip -r GViewer_$TODAY.zip \
$GVS_DIR/AUTHORS \
$GVS_DIR/README \
$GVS_DIR/gdefs.h \
$GVS_DIR/gviewer_m4d.pri \
$GVS_DIR/gviewer_m4d.pro \
$GVS_DIR/gviewer.qrc \
$GVS_DIR/gviewer_main.cpp \
$GVS_DIR/doc/examples/*.ini \
$GVS_DIR/doc/examples/*.obj \
$GVS_DIR/doc/examples/*.cfg \
$GVS_DIR/doc/examples/*.png \
$GVS_DIR/doc/examples/html/*.html \
$GVS_DIR/doc/manual/*.html \
$GVS_DIR/doc/manual/gv.css \
$GVS_DIR/doc/manual/html/*.html \
$GVS_DIR/doc/pics/*.png \
$GVS_DIR/data/box_ball.obj \
$GVS_DIR/data/coords.obj \
$GVS_DIR/data/objects.obj \
$GVS_DIR/data/Tutorial/*.ini \
$GVS_DIR/data/Tutorial/*.cfg \
$GVS_DIR/data/Tutorial/*.obj \
$GVS_DIR/icons/*.png  \
$GVS_DIR/icons/readme \
$GVS_DIR/model/*.h \
$GVS_DIR/model/*.cpp \
$GVS_DIR/utils/*.h \
$GVS_DIR/utils/*.cpp \
$GVS_DIR/view/*.h \
$GVS_DIR/view/*.cpp \
$GVS_DIR/shader/frag.c \
$GVS_DIR/shader/vert.c \
$M4D_DIR/m4d_sources.pri \
$M4D_DIR/src/stdafx.h \
$M4D_DIR/src/stdafx.cpp \
$M4D_DIR/src/targetver.h \
$M4D_DIR/src/m4dGlobalDefs.h \
$M4D_DIR/src/extra/*.h \
$M4D_DIR/src/extra/*.cpp \
$M4D_DIR/src/math/*.h \
$M4D_DIR/src/math/*.cpp \
$M4D_DIR/src/metric/*.h \
$M4D_DIR/src/metric/*.cpp \
$M4D_DIR/src/motion/*.h \
$M4D_DIR/src/motion/*.cpp

cd $GVS_DIR

