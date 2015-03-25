// --------------------------------------------------------------------------------
/*
    opengl2d_model.h

  Copyright (c) 2010  Thomas Mueller


   This file is part of the GeodesicViewer.

   The GeodesicViewer is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   The GeodesicViewer is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GeodesicViewer.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!  \class  OpenGL2dModel
     \brief  Render engine for the 2d representation of geodesics based on OpenGL.

*/
// --------------------------------------------------------------------------------

#ifndef OPENGL2D_MODEL_H
#define OPENGL2D_MODEL_H

#include <QGLWidget>
#include <QMouseEvent>

#include <gdefs.h>
#include <utils/myobject.h>
#include <utils/utilities.h>

#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>
#include <extra/m4dObject.h>


// ---------------------------------------------------
//    class definition:   OpenGL2dModel
// ---------------------------------------------------
class OpenGL2dModel : public QGLWidget {
    Q_OBJECT

public:
    OpenGL2dModel(struct_params* par, QWidget* parent = 0);
    virtual ~OpenGL2dModel();

// --------- public methods -----------
public:
    void  setPoints(m4d::enum_draw_type  dtype, bool update = true);
    void  clearPoints();
    void  setAbsOrd(enum_draw_coord_num  absNum,  enum_draw_coord_num  ordNum);

    void  setScaling(double xMin, double xMax, double yMin, double yMax);
    void  getScaling(double &xMin, double &xMax, double &yMin, double &yMax);

    void  setStepIdx(int xidx, int yidx);
    void  getStepIdx(int &xidx, int &yidx);

    void  clearAllObjects();
    void  insertObject(MyObject* obj);

    void  setFGcolor(QColor col);
    void  setBGcolor(QColor col);
    void  setGridColor(QColor col);
    void  setColors(QColor fgcol, QColor bgcol, QColor gridcol);

    void  setLineWidth(int width);
    void  setLineSmooth(int smooth);
    void  setStyle(enum_draw_style  style);

    bool  saveRGBimage(QString filename);
    void  updateParams();

    void  getCurrPos(double &x, double &y);

    void  showNumVerts(int num);

    Q_INVOKABLE void  center();
    Q_INVOKABLE void  reset();


// ------------ signals ---------------
signals:
    void  scalingChanged();
    void  scalingReset();

// -------- protected methods ---------
protected:
    virtual void  initializeGL();
    virtual void  paintGL();
    virtual void  drawLattice();

    virtual void  keyPressEvent(QKeyEvent* event);
    virtual void  keyReleaseEvent(QKeyEvent* event);

    virtual void  mousePressEvent(QMouseEvent* event);
    virtual void  mouseReleaseEvent(QMouseEvent* event);
    virtual void  mouseMoveEvent(QMouseEvent* event);

    void   getXY(QPoint pos, double &x, double &y);
    void   adjust();
    void   getTightLattice();
    void   setLattice();

// -------- private attributes --------
private:
    struct_params*     mParams;

    enum_mouse_handle  mMouseHandle;
    enum_draw_style    mDrawStyle;

    QPoint             mLastPos;
    int                mKeyPressed;
    Qt::MouseButton    mButtonPressed;

    QFont              mTicksFont;
    QColor             mFGcolor;
    QColor             mBGcolor;
    QColor             mGridColor;

    GLfloat*           mVerts;
    int                mNumVerts;
    int                mLineWidth;
    int                mLineSmooth;
    int                mShowNumVerts;

    m4d::enum_draw_type   mDrawType;

    std::vector<MyObject*>   mObjects;

    int       xStart,xEnd;
    int       yStart,yEnd;
    double    mXmin, mXmax, mYmin, mYmax;
    double    mXstep, mYstep;
    int       mXstepIdx, mYstepIdx;
    double    mFactorX, mFactorY;
    double    mAspect;

    QList<double>  mStepList;

    double    mZoomXul, mZoomYul, mZoomXlr, mZoomYlr;
    bool      mShowZoom;
    QColor    mZoomColor;

    enum_draw_coord_num   mAbscissa;
    enum_draw_coord_num   mOrdinate;
};

#endif
