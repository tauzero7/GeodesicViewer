@rem -------------------------------------------------
@rem   Batch script to run Geodesicviewer on Windows
@rem -------------------------------------------------

@rem Set the path to the Qt installation
@set QT_DIR=C:\Qt\Qt5.9.8\5.9.8

@rem Set path variable (note the order of the paths, otherwise it
@rem  could cause conflicts of different Qt libraries)
@set PATH=%QT_DIR%\msvc2017_64\bin;%QT_DIR%\msvc2017_64\;%PATH%

.\GeodesicViewerd.exe %*
