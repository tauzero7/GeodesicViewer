// --------------------------------------------------------------------------------
/*
    opengl2d_model.cpp

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
// -------------------------------------------------------------------------------

#include <fstream>
#include "opengl2d_model.h"
#include <math/TransCoordinates.h>

extern m4d::Object mObject;

/*! Standard constructor.
 *  \param par : pointer to struct.
 *  \param parent : pointer to parent widget.
 */
OpenGL2dModel::OpenGL2dModel(struct_params* par, QWidget* parent)
    : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::AlphaChannel), parent) {
    mParams = par;

    mButtonPressed = Qt::NoButton;
    setFocusPolicy(Qt::ClickFocus);
    mKeyPressed = Qt::Key_No;

    setMinimumSize(DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT);
    setMaximumSize(DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT);

    mBGcolor   = mParams->draw2d_bg_color;
    mFGcolor   = mParams->draw2d_line_color;
    mLineWidth = mParams->draw2d_line_width;
    mDrawStyle = enum_draw_lines;

    // Vertices for the geodesic.
    mVerts        = nullptr;
    mNumVerts     = 0;
    mShowNumVerts = 0;
    mDrawType     = m4d::enum_draw_pseudocart;

    mAspect = (DEF_OPENGL_HEIGHT - DEF_DRAW2D_BOTTOM_BORDER) / (double)(DEF_OPENGL_WIDTH - DEF_DRAW2D_LEFT_BORDER);
    mXmin = mParams->draw2d_xMin;
    mXmax = mParams->draw2d_xMax;
    mYmin = mParams->draw2d_yMin;
    mYmax = mParams->draw2d_yMax;
    mShowZoom  = false;

    mXstepIdx = DEF_DRAW2D_X_STEP;
    mYstepIdx = DEF_DRAW2D_Y_STEP;
    mXstep = dbl_draw2d_steps[mXstepIdx].toDouble();
    mYstep = dbl_draw2d_steps[mYstepIdx].toDouble();

    if (!mStepList.isEmpty()) {
        mStepList.clear();
    }
    for (int i = 0; i < dbl_draw2d_steps.size(); i++) {
        mStepList.append(dbl_draw2d_steps[i].toDouble());
    }
    adjust();
    setLattice();

    if (!mObjects.empty()) {
        mObjects.clear();
    }

    mAbscissa = enum_draw_coord_x0;
    mOrdinate = enum_draw_coord_x3;

    mTicksFont = QFont("Helvetica", 9);

    setMouseTracking(true);
}

/*! Standard destructor.
 */
OpenGL2dModel::~OpenGL2dModel() {
}

// ************************************ public methods ********************************
/*! Set geodesic points.
 *  \param  data : pointer to object.
 *  \param  dtype : type of drawing.
 *  \param  update : update gl.
 */
void OpenGL2dModel::setPoints(m4d::enum_draw_type  dtype, bool update) {
    if (mVerts != nullptr) {
        delete [] mVerts;
    }
    mVerts = nullptr;
    mNumVerts     = 0;
    mShowNumVerts = 0;
    mDrawType     = dtype;

    if (dtype == m4d::enum_draw_effpoti) {
        if (update) {
            updateGL();
        }
        return;
    }

    mNumVerts = static_cast<int>(mObject.points.size());
    mVerts    = new GLfloat[ mNumVerts * 2 ];

    GLfloat* vptr = mVerts;

    m4d::vec4 tp;
    for (int i = 0; i < mNumVerts; i++) {
        switch (dtype) {
            case m4d::enum_draw_pseudocart: {
                mObject.currMetric->transToPseudoCart(mObject.points[i], tp);
                *(vptr++) = GLfloat(tp[1]);
                *(vptr++) = GLfloat(tp[2]);
                break;
            }
            case m4d::enum_draw_coordinates: {
                switch (mAbscissa) {
                    case enum_draw_coord_x0:
                    case enum_draw_coord_x1:
                    case enum_draw_coord_x2:
                    case enum_draw_coord_x3:
                        *(vptr++) = GLfloat(mObject.points[i].x((int)mAbscissa));
                        break;
                    case enum_draw_lambda:
                        *(vptr++) = GLfloat(mObject.lambda[i]);
                        break;
                    case enum_draw_coord_dx0:
                    case enum_draw_coord_dx1:
                    case enum_draw_coord_dx2:
                    case enum_draw_coord_dx3:
                        *(vptr++) = GLfloat(mObject.dirs[i].x(((int)mAbscissa) - 5));
                        break;
                }
                switch (mOrdinate) {
                    case enum_draw_coord_x0:
                    case enum_draw_coord_x1:
                    case enum_draw_coord_x2:
                    case enum_draw_coord_x3:
                        *(vptr++) = GLfloat(mObject.points[i].x((int)mOrdinate));
                        break;
                    case enum_draw_lambda:
                        *(vptr++) = GLfloat(mObject.lambda[i]);
                        break;
                    case enum_draw_coord_dx0:
                    case enum_draw_coord_dx1:
                    case enum_draw_coord_dx2:
                    case enum_draw_coord_dx3:
                        *(vptr++) = GLfloat(mObject.dirs[i].x(((int)mOrdinate) - 5));
                        break;
                }
                break;
            }
            case m4d::enum_draw_embedding:
                break;
            case m4d::enum_draw_twoplusone:
                break;
            case m4d::enum_draw_effpoti:
                break;
            case m4d::enum_draw_custom: {
                mObject.currMetric->transToCustom(mObject.points[i], tp);
                *(vptr++) = GLfloat(tp[1]);
                *(vptr++) = GLfloat(tp[2]);
                break;
            }
        }
    }

    mShowNumVerts = mNumVerts;
    if (update) {
        updateGL();
    }
}

/*! Delete points.
 */
void
OpenGL2dModel::clearPoints() {
    if (mVerts != nullptr) {
        delete [] mVerts;
    }
    mVerts = nullptr;

    mNumVerts = 0;
    mShowNumVerts = 0;
    updateGL();
}

/*!
 */
void
OpenGL2dModel::setAbsOrd(enum_draw_coord_num  absNum,  enum_draw_coord_num  ordNum) {
    mAbscissa = absNum;
    mOrdinate = ordNum;
}

/*! Set scaling parameters
 *  \param xMin : xmin
 *  \param xMax : xmax
 *  \param yMin : ymin
 *  \param yMax : ymax
 */
void
OpenGL2dModel::setScaling(double xMin, double xMax, double yMin, double yMax) {
    mXmin = xMin;
    mXmax = xMax;
    mYmin = yMin;
    mYmax = yMax;
    mAspect = (mYmax - mYmin) / (mXmax - mXmin);

    adjust();
    setLattice();
    updateGL();
}

/*! Set scaling parameters
 *  \param xMin : reference to xmin
 *  \param xMax : reference to xmax
 *  \param yMin : reference to ymin
 *  \param yMax : reference to ymax
 */
void
OpenGL2dModel::getScaling(double &xMin, double &xMax, double &yMin, double &yMax) {
    xMin = mXmin;
    xMax = mXmax;
    yMin = mYmin;
    yMax = mYmax;
}

/*! Center view
 */
void
OpenGL2dModel::center() {
    double deltaX = (mXmax - mXmin) * 0.5;
    double deltaY = (mYmax - mYmin) * 0.5;

    mXmin = -deltaX;
    mXmax = +deltaX;
    mYmin = -deltaY;
    mYmax = +deltaY;
    setLattice();
}

/*!
 */
void
OpenGL2dModel::setStepIdx(int xidx, int yidx) {
    mXstepIdx = xidx;
    mYstepIdx = yidx;
    mXstep = dbl_draw2d_steps[xidx].toDouble();
    mYstep = dbl_draw2d_steps[yidx].toDouble();
    setLattice();
    updateGL();
}

void
OpenGL2dModel::getStepIdx(int &xidx, int &yidx) {
    xidx = mXstepIdx;
    yidx = mYstepIdx;
}

/*! Clear all objects.
 */
void
OpenGL2dModel::clearAllObjects() {
    if (!mObjects.empty()) {
        mObjects.clear();
    }
    updateGL();
}

/*! Insert object.
 *  \param obj : pointer to object
 */
void
OpenGL2dModel::insertObject(MyObject* obj) {
    makeCurrent();
    MyObject* no = new MyObject(*obj);
    mObjects.push_back(no);

    updateGL();
}

/*! Set foreground color.
 *  \param  col : foreground color.
 */
void
OpenGL2dModel::setFGcolor(QColor col) {
    mFGcolor = col;
    updateGL();
}

/*! Set background color.
 *  \param  col : background color.
 */
void
OpenGL2dModel::setBGcolor(QColor col) {
    mBGcolor = col;
    updateGL();
}

/*! Set grid color.
 *  \param  col : grid color.
 */
void
OpenGL2dModel::setGridColor(QColor col) {
    mGridColor = col;
    updateGL();
}

/*! Set all colors.
 * \param fgcol : foreground color.
 * \param bgcol : background color.
 * \param gridcol : grid color.
 */
void
OpenGL2dModel::setColors(QColor fgcol, QColor bgcol, QColor gridcol) {
    mBGcolor = bgcol;
    mFGcolor = fgcol;
    mGridColor = gridcol;
    updateGL();
}

/*! Set line width.
 *  \param width : line width/point size.
 */
void
OpenGL2dModel::setLineWidth(int width) {
    mLineWidth = width;
    updateGL();
}

/*! Set line anti-aliasing.
 *  \param smooth : 1=doAntialiasing
 */
void
OpenGL2dModel::setLineSmooth(int smooth) {
    mLineSmooth = smooth;
    updateGL();
}

/*! Set drawing style.
 *  \param  style : drawing style.
 *  \sa enum_draw_style.
 */
void
OpenGL2dModel::setStyle(enum_draw_style  style) {
    mDrawStyle = style;
}

/*! Set number of vertices do be drawn.
 *  \param num : number of vertices to be drawn.
 */
void
OpenGL2dModel::showNumVerts(int num) {
    mShowNumVerts = num;
    if (mShowNumVerts < 0) {
        mShowNumVerts = 0;
    } else if (mShowNumVerts > mNumVerts) {
        mShowNumVerts = mNumVerts;
    }
    updateGL();
}

/*! Save rgb image.
 *  \param filename : image name.
 */
bool
OpenGL2dModel::saveRGBimage(QString filename) {
    unsigned char* buf = new unsigned char[DEF_OPENGL_WIDTH * DEF_OPENGL_HEIGHT * 4];

    updateGL();
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buf);

    QImage img(buf, DEF_OPENGL_WIDTH, DEF_OPENGL_HEIGHT, QImage::Format_RGB888);
    QTransform trans;
    trans.scale(1.0, -1.0);
    QImage flippedImage = img.transformed(trans);
    if (!flippedImage.isNull()) {
        flippedImage.save(filename);
    }

    /*
    std::ofstream out(filename.toStdString().c_str());
    if (!out.is_open())
    {
    delete [] buf;
    return false;
    }

    out << "P6" << std::endl << DEF_OPENGL_WIDTH << " " << DEF_OPENGL_HEIGHT << std::endl << "255" << std::endl;
    int num;
    for(int row=DEF_OPENGL_HEIGHT-1; row>=0; row--)
    for(int col=0; col<DEF_OPENGL_WIDTH; col++)
    {
      num = (row*DEF_OPENGL_WIDTH+col)*4;
      out << char(buf[num+0]) << char(buf[num+1]) << char(buf[num+2]);
    }
    out << std::endl;
    out.close();
    */

    delete [] buf;
    return true;
}

/*!  Update viewing parameters.
 */
void
OpenGL2dModel::updateParams() {
    mLineWidth  = mParams->draw2d_line_width;
    mLineSmooth = mParams->draw2d_line_smooth;
    mBGcolor    = mParams->draw2d_bg_color;
    mFGcolor    = mParams->draw2d_line_color;
    mGridColor  = mParams->draw2d_grid_color;

    mAbscissa   = (enum_draw_coord_num)mParams->draw2d_abscissa;
    mOrdinate   = (enum_draw_coord_num)mParams->draw2d_ordinate;

    adjust();
    getTightLattice();
    setLattice();
    updateGL();
}

/*!  Get current position.
 *  \param x : reference to x position.
 *  \param y : reference to y position.
 */
void
OpenGL2dModel::getCurrPos(double &x, double &y) {
    getXY(mLastPos, x, y);
}

/*! Reset.
 */
void
OpenGL2dModel::reset() {
    clearAllObjects();
    clearPoints();
    mDrawType = m4d::enum_draw_pseudocart;
    updateParams();
    updateGL();
}

// *********************************** protected methods ******************************
/*!
 *
 */
void
OpenGL2dModel::initializeGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

/*!
 */
void
OpenGL2dModel::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* -----------------------
    *   draw ticks
    * ----------------------- */
    glColor3f(1, 1, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(mXmin, mXmax, 0, 1);
    glViewport(DEF_DRAW2D_LEFT_BORDER, 0, DEF_OPENGL_WIDTH - DEF_DRAW2D_LEFT_BORDER, DEF_DRAW2D_BOTTOM_BORDER);
    for (int x = xStart; x < xEnd; x++) {
        glBegin(GL_LINES);
        glVertex2f((float)(x * mXstep), 0.6f);
        glVertex2f((float)(x * mXstep), 1.0f);
        glEnd();
        renderText(x * mXstep, 0.1, 0.0, QString::number(x * mXstep), mTicksFont);
    }

    glLoadIdentity();
    gluOrtho2D(0, 1, mYmin, mYmax);
    glViewport(0, DEF_DRAW2D_BOTTOM_BORDER, DEF_DRAW2D_LEFT_BORDER, DEF_OPENGL_HEIGHT - DEF_DRAW2D_BOTTOM_BORDER);
    for (int y = yStart; y < yEnd; y++) {
        glBegin(GL_LINES);
        glVertex2f(0.6f, y * mYstep);
        glVertex2f(1.0f, y * mYstep);
        glEnd();
        renderText(0.1, y * mYstep, 0.0, QString::number(y * mYstep), mTicksFont);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(mXmin, mXmax, mYmin, mYmax);
    glViewport(DEF_DRAW2D_LEFT_BORDER, DEF_DRAW2D_BOTTOM_BORDER, DEF_OPENGL_WIDTH - DEF_DRAW2D_LEFT_BORDER, DEF_OPENGL_HEIGHT - DEF_DRAW2D_BOTTOM_BORDER);

    /* -----------------------
    *   draw background
    * ----------------------- */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(mBGcolor.redF(), mBGcolor.greenF(), mBGcolor.blueF());
    glBegin(GL_QUADS);
    glVertex2f(mXmin, mYmin);
    glVertex2f(mXmax, mYmin);
    glVertex2f(mXmax, mYmax);
    glVertex2f(mXmin, mYmax);
    glEnd();

    /* -----------------------
    *   draw lattice
    * ----------------------- */
    drawLattice();

    /* -----------------------
    *   draw objects
    * ----------------------- */
    for (unsigned int i = 0; i < mObjects.size(); i++) {
        if (mObjects[i]->getObjectDim() == enum_object_dim_2d) {
            if (!mObjects[i]->drawObject(false)) {
                if (mObjects[i]->getObjectType() == enum_object_text2d && mObjects[i]->getObjectDim() == enum_object_dim_2d) {
                    float cx, cy, size;
                    mObjects[i]->getValue(0, cx);
                    mObjects[i]->getValue(1, cy);
                    mObjects[i]->getValue(2, size);
                    QFont font;
                    font.setPixelSize((int)size);
                    this->renderText((double)cx, (double)cy, 0.0, QString(mObjects[i]->getText().c_str()), font);
                }
            }
        }
    }

    /* -----------------------
    *   draw geodesic
    * ----------------------- */
    glLineWidth(mLineWidth);
    if (mLineSmooth == 1) {
        glEnable(GL_LINE_SMOOTH);
    }

    glColor3f(mFGcolor.redF(), mFGcolor.greenF(), mFGcolor.blueF());

    glPointSize(mLineWidth);
    glVertexPointer(2, GL_FLOAT, 0, mVerts);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (mDrawStyle == enum_draw_lines) {
        glDrawArrays(GL_LINE_STRIP, 0, mShowNumVerts);
    } else {
        glDrawArrays(GL_POINTS, 0, mShowNumVerts);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glLineWidth(1);
    glDisable(GL_LINE_SMOOTH);

    /* -----------------------
    *   draw effective
    * ----------------------- */
    if (mDrawType == m4d::enum_draw_effpoti) {
        if (mObject.currMetric != nullptr) {
            double xstep = (mXmax - mXmin) / (double)DEF_DRAW2D_WIDTH;
            glColor3f(1, 0, 0);
            glBegin(GL_LINE_STRIP);
            for (double x = mXmin; x < mXmax; x += xstep) {
                double val;
                if (mObject.currMetric->effPotentialValue(mObject.startPos, mObject.coordDir, mObject.type, x, val)) {
                    glVertex2f(x, val);
                }
            }
            glEnd();

            double k;
            if (mObject.currMetric->totEnergy(mObject.startPos, mObject.coordDir, 0.0, k)) {
                glColor3f(0, 0, 1);
                glBegin(GL_LINES);
                glVertex2f(mXmin, k);
                glVertex2f(mXmax, k);
                glEnd();
            }
        }
    }

    /* -----------------------
    *   draw zoom quad
    * ----------------------- */
    if (mShowZoom) {
        glColor4f(1.0f - mBGcolor.redF(), 1.0f - mBGcolor.greenF(), 1.0f - mBGcolor.blueF(), 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(mZoomXul, mZoomYlr);
        glVertex2f(mZoomXlr, mZoomYlr);
        glVertex2f(mZoomXlr, mZoomYul);
        glVertex2f(mZoomXul, mZoomYul);
        glEnd();
    }
}

/*!
 */
void
OpenGL2dModel::drawLattice() {
    glLineStipple(1, 0x1111);
    glEnable(GL_LINE_STIPPLE);

    glColor3f(mGridColor.redF(), mGridColor.greenF(), mGridColor.blueF());
    for (int x = xStart; x < xEnd; x++) {
        glBegin(GL_LINES);
        glVertex2f(x * mXstep, mYmin);
        glVertex2f(x * mXstep, mYmax);
        glEnd();
    }
    for (int y = yStart; y < yEnd; y++) {
        glBegin(GL_LINES);
        glVertex2f(mXmin, y * mYstep);
        glVertex2f(mXmax, y * mYstep);
        glEnd();
    }
    glDisable(GL_LINE_STIPPLE);

    // 'origin' cross
    glBegin(GL_LINES);
    glVertex2f(0.0, mYmin);
    glVertex2f(0.0, mYmax);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(mXmin, 0.0);
    glVertex2f(mXmax, 0.0);
    glEnd();
}

/*!
 *  \param event : key event.
 */
void
OpenGL2dModel::keyPressEvent(QKeyEvent* event) {
    mKeyPressed = event->key();

    if (mKeyPressed == Qt::Key_C) {
        center();
        emit scalingChanged();
    } else if (mKeyPressed == Qt::Key_R) {
        emit scalingReset();
    } else if (mKeyPressed == Qt::Key_PageDown) {
        double zoomFactor = (mXmax - mXmin) * 0.1;
        mXmin -= zoomFactor;
        mXmax += zoomFactor;
        mYmin -= zoomFactor * mAspect;
        mYmax += zoomFactor * mAspect;
        adjust();
        setLattice();
    } else if (mKeyPressed == Qt::Key_PageUp) {
        double zoomFactor = (mXmax - mXmin) * 0.1;
        mXmin += zoomFactor;
        mXmax -= zoomFactor;
        mYmin += zoomFactor * mAspect;
        mYmax -= zoomFactor * mAspect;
        adjust();
        setLattice();
    }
    updateGL();
}

/*!
 *  \param event : key event.
 */
void
OpenGL2dModel::keyReleaseEvent(QKeyEvent* event) {
    mKeyPressed = Qt::Key_No;
    event->ignore();
}

/*!
 *  \param event : mouse event.
 */
void
OpenGL2dModel::mousePressEvent(QMouseEvent * event) {
    mButtonPressed = event->button();
    mLastPos = event->pos();

    if (mButtonPressed == Qt::RightButton) {
        getXY(mLastPos, mZoomXul, mZoomYul);
        mZoomXlr = mZoomXul;
        mZoomYlr = mZoomYul;
        mShowZoom = true;
    }
}

/*!
 *  \param event : mouse event.
 */
void
OpenGL2dModel::mouseReleaseEvent(QMouseEvent * event) {
    mButtonPressed = Qt::NoButton;
    event->accept();

    if (mShowZoom) {
        if (mZoomXul < mZoomXlr && mZoomYul > mZoomYlr) {
            mXmin = mZoomXul;
            mXmax = mZoomXlr;
            mYmin = mZoomYlr;
            mYmax = mZoomYul;
        } else if (mZoomXul > mZoomXlr && mZoomYul < mZoomYlr) {
            mXmin -= fabs(mZoomXlr - mXmin);
            mXmax += fabs(mZoomXul - mXmax);
            mYmin -= fabs(mZoomYlr - mYmin);
            mYmax += fabs(mZoomYul - mYmax);
        }
        mAspect = (mYmax - mYmin) / (mXmax - mXmin);
        adjust();
        getTightLattice();
        setLattice();
        mShowZoom = false;
        emit scalingChanged();
        updateGL();
    }
}

/*!
 *  \param event : mouse event.
 */
void
OpenGL2dModel::mouseMoveEvent(QMouseEvent * event) {
    QPoint dxy = event->pos() - mLastPos;
    mLastPos   = event->pos();

    if (mButtonPressed == Qt::LeftButton) {
        mXmin -= dxy.x() * mFactorX;
        mXmax -= dxy.x() * mFactorX;
        mYmin += dxy.y() * mFactorY;
        mYmax += dxy.y() * mFactorY;
        setLattice();
    } else if (mButtonPressed == Qt::MidButton) {
        double zoomFactor = 0.05;
        mXmin -= dxy.y() * zoomFactor;
        mXmax += dxy.y() * zoomFactor;
        mYmin -= dxy.y() * zoomFactor * mAspect;
        mYmax += dxy.y() * zoomFactor * mAspect;
        adjust();
        setLattice();
    } else if (mButtonPressed == Qt::RightButton) {
        getXY(mLastPos, mZoomXlr, mZoomYlr);
    }

    mParams->draw2d_xMin = mXmin;
    mParams->draw2d_xMax = mXmax;
    mParams->draw2d_yMin = mYmin;
    mParams->draw2d_yMax = mYmax;
    emit scalingChanged();
    updateGL();
}

/*!
 */
void
OpenGL2dModel::getXY(QPoint pos, double &x, double &y) {
    x = (double)(pos.x() - DEF_DRAW2D_LEFT_BORDER) / (DEF_OPENGL_WIDTH - DEF_DRAW2D_LEFT_BORDER) * (mXmax - mXmin) + mXmin;
    y = (double)(DEF_OPENGL_HEIGHT - DEF_DRAW2D_BOTTOM_BORDER - pos.y()) / (DEF_OPENGL_HEIGHT - DEF_DRAW2D_BOTTOM_BORDER) * (mYmax - mYmin) + mYmin;
}

/*!
 */
void
OpenGL2dModel::adjust() {
    mFactorX = (mXmax - mXmin) / (double)(DEF_OPENGL_WIDTH - DEF_DRAW2D_LEFT_BORDER);
    mFactorY = (mYmax - mYmin) / (double)(DEF_OPENGL_HEIGHT - DEF_DRAW2D_BOTTOM_BORDER);
}

/*! Get tight lattice
 */
void
OpenGL2dModel::getTightLattice() {
    double pXstep = (mXmax - mXmin) / (double)DEF_DRAW2D_X_STEP;
    double pYstep = (mYmax - mYmin) / (double)DEF_DRAW2D_Y_STEP;

    int idx = 0, idy = 0;
    double dist, minDist = 1e32;
    for (mXstepIdx = 0; mXstepIdx < mStepList.size(); mXstepIdx++) {
        dist = fabs(pXstep - mStepList[mXstepIdx]);
        if (dist < minDist) {
            minDist = dist;
            idx = mXstepIdx;
        }
    }
    minDist = 1e32;
    for (mYstepIdx = 0; mYstepIdx < mStepList.size(); mYstepIdx++) {
        dist = fabs(pYstep - mStepList[mYstepIdx]);
        if (dist < minDist) {
            minDist = dist;
            idy = mYstepIdx;
        }
    }
    mXstepIdx = idx;
    mYstepIdx = idy;
    mXstep = mStepList[mXstepIdx];
    mYstep = mStepList[mYstepIdx];
}

/*!
 */
void
OpenGL2dModel::setLattice() {
    //bool hasChanged = false;

    int pixStepX = floor(mXstep / mFactorX);
    int pixStepY = floor(mYstep / mFactorY);

    if (pixStepX > 119) {
        mXstepIdx--;
        if (mXstepIdx < 0) {
            mXstepIdx = 0;
        }
        //hasChanged = true;
    }
    if (pixStepX < 40) {
        mXstepIdx++;
        if (mXstepIdx >= mStepList.size()) {
            mXstepIdx = mStepList.size() - 1;
        }
        //hasChanged = true;
    }

    if (pixStepY > 119) {
        mYstepIdx--;
        if (mYstepIdx < 0) {
            mYstepIdx = 0;
        }
        //hasChanged = true;
    }
    if (pixStepY < 40) {
        mYstepIdx++;
        if (mYstepIdx >= mStepList.size()) {
            mYstepIdx = mStepList.size() - 1;
        }
        //hasChanged = true;
    }

    mXstep = mStepList[mXstepIdx];
    mYstep = mStepList[mYstepIdx];

    // fprintf(stderr,"%d %d  %d %d  %f %f\n",pixStepX,pixStepY,mXstepIdx,mYstepIdx,mXstep,mYstep);

    xStart = floor(mXmin / mXstep) + 1;
    xEnd   = floor(mXmax / mXstep) + 1;
    yStart = floor(mYmin / mYstep) + 1;
    yEnd   = floor(mYmax / mYstep) + 1;
}
