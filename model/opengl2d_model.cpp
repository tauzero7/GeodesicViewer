/**
 * @file    opengl2d_model.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "opengl2d_model.h"
#include "math/TransCoordinates.h"
#include "utils/utilities.h"
#include <QApplication>
#include <QDesktopWidget>
#include <fstream>

extern m4d::Object mObject;

OpenGL2dModel::OpenGL2dModel(struct_params* par, QWidget* parent)
    : QOpenGLWidget(parent)
{
    mParams = par;

    mButtonPressed = Qt::NoButton;
    setFocusPolicy(Qt::ClickFocus);
    mKeyPressed = Qt::Key_No;

    mWinSize[0] = DEF_OPENGL_WIDTH;
    mWinSize[1] = DEF_OPENGL_HEIGHT;
    mAspect = (mWinSize[0] - DEF_DRAW2D_LEFT_BORDER) / static_cast<double>(mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER);

    mBGcolor = mParams->draw2d_bg_color;
    mFGcolor = mParams->draw2d_line_color;
    mLineWidth = mParams->draw2d_line_width;
    mDrawStyle = enum_draw_lines;
    mLineSmooth = 1;

    // Vertices for the geodesic.
    mVerts = nullptr;
    mNumVerts = 0;
    mShowNumVerts = 0;
    mDrawType = m4d::enum_draw_pseudocart;

    mXmin = mParams->draw2d_xMin;
    mXmax = mParams->draw2d_xMax;
    mYmin = mParams->draw2d_yMin;
    mYmax = mParams->draw2d_yMax;
    mShowZoom = false;

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

OpenGL2dModel::~OpenGL2dModel() {}

void OpenGL2dModel::setPoints(m4d::enum_draw_type dtype, bool needUpdate)
{
    SafeDelete<GLfloat>(mVerts);

    mNumVerts = 0;
    mShowNumVerts = 0;
    mDrawType = dtype;

    if (dtype == m4d::enum_draw_effpoti) {
        if (needUpdate) {
            update();
        }
        return;
    }

    mNumVerts = mObject.points.size();
    mVerts = new GLfloat[mNumVerts * 2];

    GLfloat* vptr = mVerts;

    m4d::vec4 tp;
    for (size_t i = 0; i < mNumVerts; i++) {
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
                        *(vptr++) = GLfloat(mObject.points[i].x(static_cast<int>(mAbscissa)));
                        break;
                    case enum_draw_lambda:
                        *(vptr++) = GLfloat(mObject.lambda[i]);
                        break;
                    case enum_draw_coord_dx0:
                    case enum_draw_coord_dx1:
                    case enum_draw_coord_dx2:
                    case enum_draw_coord_dx3:
                        *(vptr++) = GLfloat(mObject.dirs[i].x(static_cast<int>(mAbscissa) - 5));
                        break;
                }
                switch (mOrdinate) {
                    case enum_draw_coord_x0:
                    case enum_draw_coord_x1:
                    case enum_draw_coord_x2:
                    case enum_draw_coord_x3:
                        *(vptr++) = GLfloat(mObject.points[i].x(static_cast<int>(mOrdinate)));
                        break;
                    case enum_draw_lambda:
                        *(vptr++) = GLfloat(mObject.lambda[i]);
                        break;
                    case enum_draw_coord_dx0:
                    case enum_draw_coord_dx1:
                    case enum_draw_coord_dx2:
                    case enum_draw_coord_dx3:
                        *(vptr++) = GLfloat(mObject.dirs[i].x(static_cast<int>(mOrdinate) - 5));
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
    if (needUpdate) {
        update();
    }
}

void OpenGL2dModel::clearPoints()
{
    SafeDelete<GLfloat>(mVerts);

    mNumVerts = 0;
    mShowNumVerts = 0;
    update();
}

void OpenGL2dModel::setAbsOrd(enum_draw_coord_num absNum, enum_draw_coord_num ordNum)
{
    mAbscissa = absNum;
    mOrdinate = ordNum;
}

void OpenGL2dModel::setScaling(double xMin, double xMax, double yMin, double yMax)
{
    mXmin = xMin;
    mXmax = xMax;
    mYmin = yMin;
    mYmax = yMax;
    mAspect = (mXmax - mXmin) / (mYmax - mYmin);

    adjust();
    setLattice();
    update();
}

void OpenGL2dModel::getScaling(double& xMin, double& xMax, double& yMin, double& yMax)
{
    xMin = mXmin;
    xMax = mXmax;
    yMin = mYmin;
    yMax = mYmax;
}

void OpenGL2dModel::center()
{
    double deltaX = (mXmax - mXmin) * 0.5;
    double deltaY = (mYmax - mYmin) * 0.5;

    mXmin = -deltaX;
    mXmax = +deltaX;
    mYmin = -deltaY;
    mYmax = +deltaY;
    setLattice();
    update();
}

void OpenGL2dModel::makeUniform()
{
    double xcenter = (mXmin + mXmax) * 0.5;
    double yrange = mYmax - mYmin;
    double aspect
        = (mWinSize[0] - DEF_DRAW2D_LEFT_BORDER) / static_cast<double>(mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER);
    mXmin = xcenter - 0.5 * yrange * aspect;
    mXmax = xcenter + 0.5 * yrange * aspect;
    setScaling(mXmin, mXmax, mYmin, mYmax);
}

void OpenGL2dModel::setStepIdx(int xidx, int yidx)
{
    mXstepIdx = xidx;
    mYstepIdx = yidx;
    mXstep = dbl_draw2d_steps[xidx].toDouble();
    mYstep = dbl_draw2d_steps[yidx].toDouble();
    setLattice();
    update();
}

void OpenGL2dModel::getStepIdx(int& xidx, int& yidx)
{
    xidx = mXstepIdx;
    yidx = mYstepIdx;
}

void OpenGL2dModel::clearAllObjects()
{
    if (!mObjects.empty()) {
        mObjects.clear();
    }
    update();
}

void OpenGL2dModel::insertObject(MyObject* obj)
{
    makeCurrent();
    MyObject* no = new MyObject(*obj);
    mObjects.push_back(no);
    update();
}

void OpenGL2dModel::setFGcolor(QColor col)
{
    mFGcolor = col;
    update();
}

void OpenGL2dModel::setBGcolor(QColor col)
{
    mBGcolor = col;
    update();
}

void OpenGL2dModel::setGridColor(QColor col)
{
    mGridColor = col;
    update();
}

void OpenGL2dModel::setColors(QColor fgcol, QColor bgcol, QColor gridcol)
{
    mBGcolor = bgcol;
    mFGcolor = fgcol;
    mGridColor = gridcol;
    update();
}

void OpenGL2dModel::setLineWidth(int width)
{
    mLineWidth = width;
    update();
}

void OpenGL2dModel::setLineSmooth(int smooth)
{
    mLineSmooth = smooth;
    update();
}

void OpenGL2dModel::setStyle(enum_draw_style style)
{
    mDrawStyle = style;
}

void OpenGL2dModel::showNumVerts(int num)
{
    mShowNumVerts = static_cast<size_t>(std::max(0, std::min(num, static_cast<int>(mNumVerts))));
    update();
}

void OpenGL2dModel::getWinSize(int& width, int& height)
{
    width = mWinSize[0];
    height = mWinSize[1];
}

bool OpenGL2dModel::saveRGBimage(QString filename)
{
    unsigned char* buf = new unsigned char[DEF_OPENGL_WIDTH * DEF_OPENGL_HEIGHT * 4];

    update();
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

    delete[] buf;
    return true;
}

void OpenGL2dModel::updateParams()
{
    mLineWidth = mParams->draw2d_line_width;
    mLineSmooth = mParams->draw2d_line_smooth;
    mBGcolor = mParams->draw2d_bg_color;
    mFGcolor = mParams->draw2d_line_color;
    mGridColor = mParams->draw2d_grid_color;

    mAbscissa = static_cast<enum_draw_coord_num>(mParams->draw2d_abscissa);
    mOrdinate = static_cast<enum_draw_coord_num>(mParams->draw2d_ordinate);

    adjust();
    getTightLattice();
    setLattice();
    update();
}

void OpenGL2dModel::getCurrPos(double& x, double& y)
{
    getXY(mLastPos, x, y);
}

void OpenGL2dModel::reset()
{
    clearAllObjects();
    clearPoints();
    mDrawType = m4d::enum_draw_pseudocart;
    updateParams();
    update();
}

void OpenGL2dModel::initializeGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    mDPIFactor[0] = QApplication::desktop()->devicePixelRatioF();
    mDPIFactor[1] = QApplication::desktop()->devicePixelRatioF();

    renderText = new RenderText("resources/DroidSansMono.ttf", 12);
}

void OpenGL2dModel::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // -----------------------
    //   draw ticks
    // -----------------------
    glColor3f(1, 1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(mXmin, mXmax, 0, 1);
    glViewport(DEF_DRAW2D_LEFT_BORDER, 0, mWinSize[0] - DEF_DRAW2D_LEFT_BORDER, DEF_DRAW2D_BOTTOM_BORDER);
    for (int x = xStart; x < xEnd; x++) {
        glBegin(GL_LINES);
        glVertex2f(static_cast<float>(x * mXstep), 0.6f);
        glVertex2f(static_cast<float>(x * mXstep), 1.0f);
        glEnd();
    }

    glLoadIdentity();
    gluOrtho2D(0, 1, mYmin, mYmax);
    glViewport(0, DEF_DRAW2D_BOTTOM_BORDER, DEF_DRAW2D_LEFT_BORDER, mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER);
    for (int y = yStart; y < yEnd; y++) {
        glBegin(GL_LINES);
        glVertex2f(0.6f, static_cast<float>(y * mYstep));
        glVertex2f(1.0f, static_cast<float>(y * mYstep));
        glEnd();
        // renderText(0.1, y * mYstep, 0.0, QString::number(y * mYstep), mTicksFont);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(mXmin, mXmax, mYmin, mYmax);
    glViewport(DEF_DRAW2D_LEFT_BORDER, DEF_DRAW2D_BOTTOM_BORDER, mWinSize[0] - DEF_DRAW2D_LEFT_BORDER,
        mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER);

    // -----------------------
    //   draw background
    // -----------------------
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3d(mBGcolor.redF(), mBGcolor.greenF(), mBGcolor.blueF());
    glBegin(GL_QUADS);
    glVertex2f(static_cast<float>(mXmin), static_cast<float>(mYmin));
    glVertex2f(static_cast<float>(mXmax), static_cast<float>(mYmin));
    glVertex2f(static_cast<float>(mXmax), static_cast<float>(mYmax));
    glVertex2f(static_cast<float>(mXmin), static_cast<float>(mYmax));
    glEnd();

    // -----------------------
    //   draw lattice
    // -----------------------
    drawLattice();

    // -----------------------
    //   draw objects
    // -----------------------
    for (unsigned int i = 0; i < mObjects.size(); i++) {
        if (mObjects[i]->getObjectDim() == enum_object_dim_2d) {
            if (!mObjects[i]->drawObject(false)) {
                if (mObjects[i]->getObjectType() == enum_object_text2d
                    && mObjects[i]->getObjectDim() == enum_object_dim_2d) {
                    double cx, cy, size;
                    mObjects[i]->getValue(0, cx);
                    mObjects[i]->getValue(1, cy);
                    mObjects[i]->getValue(2, size);
                    QFont font;
                    font.setPixelSize(static_cast<int>(size));
                    // this->renderText((double)cx, (double)cy, 0.0, QString(mObjects[i]->getText().c_str()), font);
                }
            }
        }
    }

    // -----------------------
    //  draw geodesic
    // -----------------------
    glLineWidth(mLineWidth);
    if (mLineSmooth == 1) {
        glEnable(GL_LINE_SMOOTH);
    }

    glColor3d(mFGcolor.redF(), mFGcolor.greenF(), mFGcolor.blueF());

    glPointSize(mLineWidth);
    glVertexPointer(2, GL_FLOAT, 0, mVerts);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (mDrawStyle == enum_draw_lines) {
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(mShowNumVerts));
    }
    else {
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mShowNumVerts));
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glLineWidth(1);
    glDisable(GL_LINE_SMOOTH);

    // -----------------------
    //   draw effective
    // -----------------------
    if (mDrawType == m4d::enum_draw_effpoti) {
        if (mObject.currMetric != nullptr) {
            double xstep = (mXmax - mXmin) / static_cast<double>(mWinSize[0]);
            glColor3f(1, 0, 0);
            glBegin(GL_LINE_STRIP);
            for (double x = mXmin; x < mXmax; x += xstep) {
                double val;
                if (mObject.currMetric->effPotentialValue(mObject.startPos, mObject.coordDir, mObject.type, x, val)) {
                    glVertex2f(static_cast<float>(x), static_cast<float>(val));
                }
            }
            glEnd();

            double k;
            if (mObject.currMetric->totEnergy(mObject.startPos, mObject.coordDir, 0.0, k)) {
                glColor3f(0, 0, 1);
                glBegin(GL_LINES);
                glVertex2f(static_cast<float>(mXmin), static_cast<float>(k));
                glVertex2f(static_cast<float>(mXmax), static_cast<float>(k));
                glEnd();
            }
        }
    }

    // -----------------------
    //   draw zoom quad
    // -----------------------
    if (mShowZoom) {
        glColor4d(1.0 - mBGcolor.redF(), 1.0 - mBGcolor.greenF(), 1.0 - mBGcolor.blueF(), 0.3);
        glBegin(GL_QUADS);
        glVertex2f(static_cast<float>(mZoomXul), static_cast<float>(mZoomYlr));
        glVertex2f(static_cast<float>(mZoomXlr), static_cast<float>(mZoomYlr));
        glVertex2f(static_cast<float>(mZoomXlr), static_cast<float>(mZoomYul));
        glVertex2f(static_cast<float>(mZoomXul), static_cast<float>(mZoomYul));
        glEnd();
    }

    // -----------------------
    //   draw tick labels
    // -----------------------
    if (renderText != nullptr) {
        double xScale = 1.0 / (mXmax - mXmin) * mWinSize[0];
        glViewport(DEF_DRAW2D_LEFT_BORDER, 0, mWinSize[0] - DEF_DRAW2D_LEFT_BORDER, DEF_DRAW2D_BOTTOM_BORDER);
        renderText->SetWindowSize(mWinSize[0], DEF_DRAW2D_BOTTOM_BORDER);
        for (int x = xStart; x < xEnd; x++) {
            int xpos = static_cast<int>((x * mXstep - mXmin) * xScale);
            renderText->Print(xpos, 2, QString::number(x * mXstep).toStdString().c_str(), ALIGN_HCENTER);
        }

        double yScale = 1.0 / (mYmax - mYmin) * mWinSize[1];
        glViewport(0, DEF_DRAW2D_BOTTOM_BORDER, DEF_DRAW2D_LEFT_BORDER, mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER);
        renderText->SetWindowSize(DEF_DRAW2D_LEFT_BORDER, mWinSize[1]);
        for (int y = yStart; y < yEnd; y++) {
            int ypos = static_cast<int>((y * mYstep - mYmin) * yScale);
            renderText->Print(
                DEF_DRAW2D_LEFT_BORDER - 3, ypos + 2, QString::number(y * mYstep).toStdString().c_str(), ALIGN_RIGHT);
        }
    }
}

void OpenGL2dModel::resizeGL(int width, int height)
{
    mWinSize[0] = static_cast<int>(width * mDPIFactor[0]);
    mWinSize[1] = static_cast<int>(height * mDPIFactor[1]);
    mAspect = (mWinSize[0] - DEF_DRAW2D_LEFT_BORDER) / static_cast<double>(mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER);

    double xcenter = 0.5 * (mXmax + mXmin);
    double yrange = mYmax - mYmin;
    mXmin = xcenter - 0.5 * yrange * mAspect;
    mXmax = xcenter + 0.5 * yrange * mAspect;
    adjust();
    setLattice();

    if (renderText != nullptr) {
        renderText->SetWindowSize(width, height);
    }
    update();
}

void OpenGL2dModel::drawLattice()
{
    glLineStipple(1, 0x1111);
    glEnable(GL_LINE_STIPPLE);

    glColor3d(mGridColor.redF(), mGridColor.greenF(), mGridColor.blueF());
    for (int x = xStart; x < xEnd; x++) {
        glBegin(GL_LINES);
        glVertex2f(static_cast<float>(x * mXstep), static_cast<float>(mYmin));
        glVertex2f(static_cast<float>(x * mXstep), static_cast<float>(mYmax));
        glEnd();
    }
    for (int y = yStart; y < yEnd; y++) {
        glBegin(GL_LINES);
        glVertex2f(static_cast<float>(mXmin), static_cast<float>(y * mYstep));
        glVertex2f(static_cast<float>(mXmax), static_cast<float>(y * mYstep));
        glEnd();
    }
    glDisable(GL_LINE_STIPPLE);

    // 'origin' cross
    glBegin(GL_LINES);
    glVertex2f(0.0f, static_cast<float>(mYmin));
    glVertex2f(0.0f, static_cast<float>(mYmax));
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(static_cast<float>(mXmin), 0.0f);
    glVertex2f(static_cast<float>(mXmax), 0.0f);
    glEnd();
}

void OpenGL2dModel::keyPressEvent(QKeyEvent* event)
{
    mKeyPressed = event->key();
    mModifiers = event->modifiers();

    if (mKeyPressed == Qt::Key_C) {
        center();
        emit scalingChanged();
    }
    else if (mKeyPressed == Qt::Key_R) {
        emit scalingReset();
    }
    else if (mKeyPressed == Qt::Key_S) {
        makeUniform();
        emit scalingChanged();
    }
    else if (mKeyPressed == Qt::Key_PageDown) {
        double zoomFactor = (mXmax - mXmin) * 0.1;
        mXmin -= zoomFactor * mAspect;
        mXmax += zoomFactor * mAspect;
        mYmin -= zoomFactor;
        mYmax += zoomFactor;
        adjust();
        setLattice();
    }
    else if (mKeyPressed == Qt::Key_PageUp) {
        double zoomFactor = (mXmax - mXmin) * 0.1;
        mXmin += zoomFactor * mAspect;
        mXmax -= zoomFactor * mAspect;
        mYmin += zoomFactor;
        mYmax -= zoomFactor;
        adjust();
        setLattice();
    }
    update();
}

void OpenGL2dModel::keyReleaseEvent(QKeyEvent* event)
{
    mKeyPressed = Qt::Key_No;
    mModifiers = Qt::NoModifier;
    event->ignore();
}

void OpenGL2dModel::mousePressEvent(QMouseEvent* event)
{
    mButtonPressed = event->button();
    mLastPos = event->pos();
    mLastPos.setX(static_cast<int>(mLastPos.x() * mDPIFactor[0]));
    mLastPos.setY(static_cast<int>(mLastPos.y() * mDPIFactor[1]));

    if (mButtonPressed == Qt::LeftButton && mModifiers == Qt::ControlModifier) {
        getXY(mLastPos, mZoomXul, mZoomYul);
        mZoomXlr = mZoomXul;
        mZoomYlr = mZoomYul;
        mShowZoom = true;
    }
}

void OpenGL2dModel::mouseReleaseEvent(QMouseEvent* event)
{
    mButtonPressed = Qt::NoButton;
    event->accept();

    if (mShowZoom) {
        if (mZoomXul < mZoomXlr && mZoomYul > mZoomYlr) {
            mXmin = mZoomXul;
            mXmax = mZoomXlr;
            mYmin = mZoomYlr;
            mYmax = mZoomYul;
        }
        else if (mZoomXul > mZoomXlr && mZoomYul < mZoomYlr) {
            mXmin -= fabs(mZoomXlr - mXmin);
            mXmax += fabs(mZoomXul - mXmax);
            mYmin -= fabs(mZoomYlr - mYmin);
            mYmax += fabs(mZoomYul - mYmax);
        }
        mAspect = (mXmax - mXmin) / (mYmax - mYmin);
        adjust();
        getTightLattice();
        setLattice();
        mShowZoom = false;
        emit scalingChanged();
        update();
    }
}

void OpenGL2dModel::mouseMoveEvent(QMouseEvent* event)
{
    QPoint cp = event->pos();
    cp.setX(static_cast<int>(cp.x() * mDPIFactor[0]));
    cp.setY(static_cast<int>(cp.y() * mDPIFactor[1]));
    QPoint dxy = cp - mLastPos;
    mLastPos = cp;

    if (mButtonPressed == Qt::LeftButton) {
        if (mModifiers == Qt::ControlModifier) {
            getXY(mLastPos, mZoomXlr, mZoomYlr);
        }
        else {
            mXmin -= dxy.x() * mFactorX;
            mXmax -= dxy.x() * mFactorX;
            mYmin += dxy.y() * mFactorY;
            mYmax += dxy.y() * mFactorY;
            setLattice();
        }
    }
    else if (mButtonPressed == Qt::MidButton) {
    }
    else if (mButtonPressed == Qt::RightButton) {
        double zoomFactor = 0.005 * (mYmax - mYmin);
        mXmin += dxy.y() * zoomFactor * mAspect;
        mXmax -= dxy.y() * zoomFactor * mAspect;
        mYmin += dxy.y() * zoomFactor;
        mYmax -= dxy.y() * zoomFactor;
        adjust();
        setLattice();
    }

    mParams->draw2d_xMin = mXmin;
    mParams->draw2d_xMax = mXmax;
    mParams->draw2d_yMin = mYmin;
    mParams->draw2d_yMax = mYmax;
    emit scalingChanged();
    update();
}

void OpenGL2dModel::getXY(QPoint pos, double& x, double& y)
{
    x = (pos.x() - DEF_DRAW2D_LEFT_BORDER) / static_cast<double>(mWinSize[0] - DEF_DRAW2D_LEFT_BORDER) * (mXmax - mXmin)
        + mXmin;
    y = (mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER - pos.y()) / static_cast<double>(mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER)
            * (mYmax - mYmin)
        + mYmin;
}

void OpenGL2dModel::adjust()
{
    mFactorX = (mXmax - mXmin) / static_cast<double>(mWinSize[0] - DEF_DRAW2D_LEFT_BORDER);
    mFactorY = (mYmax - mYmin) / static_cast<double>(mWinSize[1] - DEF_DRAW2D_BOTTOM_BORDER);
}

void OpenGL2dModel::getTightLattice()
{
    double pXstep = (mXmax - mXmin) / static_cast<double>(DEF_DRAW2D_X_STEP);
    double pYstep = (mYmax - mYmin) / static_cast<double>(DEF_DRAW2D_Y_STEP);

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

void OpenGL2dModel::setLattice()
{
    int pixStepX = static_cast<int>(floor(mXstep / mFactorX));
    int pixStepY = static_cast<int>(floor(mYstep / mFactorY));

    if (pixStepX > 139) {
        mXstepIdx--;
        mXstepIdx = std::max(mXstepIdx, 0);
    }
    else if (pixStepX < 51) {
        mXstepIdx++;
        mXstepIdx = std::min(mXstepIdx, mStepList.size() - 1);
    }

    if (pixStepY > 139) {
        mYstepIdx--;
        mYstepIdx = std::max(mYstepIdx, 0);
    }
    else if (pixStepY < 51) {
        mYstepIdx++;
        mYstepIdx = std::min(mYstepIdx, mStepList.size() - 1);
    }

    mXstep = mStepList[mXstepIdx];
    mYstep = mStepList[mYstepIdx];

   //  fprintf(stderr,"%d %d  %d %d  %f %f\n",pixStepX,pixStepY,mXstepIdx,mYstepIdx,mXstep,mYstep);

    xStart = static_cast<int>(floor(mXmin / mXstep)) + 1;
    xEnd = static_cast<int>(floor(mXmax / mXstep)) + 1;
    yStart = static_cast<int>(floor(mYmin / mYstep)) + 1;
    yEnd = static_cast<int>(floor(mYmax / mYstep)) + 1;
}
