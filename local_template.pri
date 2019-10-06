
## ------------------------------------------------
##  Motion4D library
## ------------------------------------------------
M4D_DIR = $$TOP_DIR/../libMotion4D/

M4D_SRC_DIR = $$M4D_DIR/src
M4D_LIB_DIR = $$M4D_DIR/lib

## ------------------------------------------------
##  GSL - Gnu Scientific Library
## ------------------------------------------------
GSL_DIR = 
GSL_LIB_DIR = $$GSL_DIR/lib

## ------------------------------------------------
##  Freetype
## ------------------------------------------------
DEFINES += HAVE_FREETYPE
FREETYPE_DIR =  
FREETYPE_LIB_DIR = $$FREETYPE_DIR/lib
INCLUDEPATH += $$FREETYPE_DIR/include/freetype2
LIBS += -L$$FREETYPE_LIB_DIR -lfreetyped


## Shows time needed to integrate
## geodesic equation
#DEFINES += SHOW_CALC_TIME


## Show status tips
#DEFINES += SHOW_STATUS_TIPS


## Use Dormand-Prince integrators
## (experimental)
#DEFINES += USE_DP_INT


## If you have LUA available, you can
## uncomment the following lines
## (experimental)
#LUA_DIR     = /usr/local/lua/5.3
#LUA_LIB_DIR = $$LUA_DIR/lib
#M4D_LUA_DIR = $$M4D_DIR/src/lua

#INCLUDEPATH += $$LUA_DIR/include
#DEFINES += HAVE_LUA
#LIBS += -L$$LUA_LIB_DIR -llua -ldl

#HEADERS += $$M4D_LUA_HEADERS
#SOURCES += $$M4D_LUA_SOURCES
