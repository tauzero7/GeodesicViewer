/**
 * @file    opengl2d_model.h
 * @author  Thomas Mueller
 *
 * @brief  Render engine for the 2d representation of geodesics based on OpenGL.
 *
 * This file is part of GeodesicView.
 */
#ifndef OPENGL2D_MODEL_H
#define OPENGL2D_MODEL_H

#include <QMouseEvent>
#include <QOpenGLWidget>

#include <gdefs.h>
#include <utils/myobject.h>
#include <utils/utilities.h>

#include <extra/m4dObject.h>
#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>

/**
 * @brief The OpenGL2dModel class
 */
class OpenGL2dModel : public QOpenGLWidget {
    Q_OBJECT

public:
    OpenGL2dModel(struct_params* par, QWidget* parent = nullptr);
    virtual ~OpenGL2dModel();

public:
    void setPoints(m4d::enum_draw_type dtype, bool needUpdate = true);
    void clearPoints();
    void setAbsOrd(enum_draw_coord_num absNum, enum_draw_coord_num ordNum);

    void setScaling(double xMin, double xMax, double yMin, double yMax);
    void getScaling(double& xMin, double& xMax, double& yMin, double& yMax);

    void setStepIdx(int xidx, int yidx);
    void getStepIdx(int& xidx, int& yidx);

    void clearAllObjects();
    void insertObject(MyObject* obj);

    void setFGcolor(QColor col);
    void setBGcolor(QColor col);
    void setGridColor(QColor col);
    void setColors(QColor fgcol, QColor bgcol, QColor gridcol);

    void setLineWidth(int width);
    void setLineSmooth(int smooth);
    void setStyle(enum_draw_style style);

    bool saveRGBimage(QString filename);
    void updateParams();

    void getCurrPos(double& x, double& y);

    void showNumVerts(int num);

    Q_INVOKABLE void center();
    Q_INVOKABLE void reset();

signals:
    void scalingChanged();
    void scalingReset();

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);
    virtual void drawLattice();

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

    void getXY(QPoint pos, double& x, double& y);
    void adjust();
    void getTightLattice();
    void setLattice();

private:
    struct_params* mParams;

    int mWinSize[2];
    double mDPIFactor[2];
    enum_mouse_handle mMouseHandle;
    enum_draw_style mDrawStyle;

    QPoint mLastPos;
    int mKeyPressed;
    Qt::MouseButton mButtonPressed;

    QFont mTicksFont;
    QColor mFGcolor;
    QColor mBGcolor;
    QColor mGridColor;

    GLfloat* mVerts;
    int mNumVerts;
    int mLineWidth;
    int mLineSmooth;
    int mShowNumVerts;

    m4d::enum_draw_type mDrawType;

    std::vector<MyObject*> mObjects;

    int xStart, xEnd;
    int yStart, yEnd;
    double mXmin, mXmax, mYmin, mYmax;
    double mXstep, mYstep;
    int mXstepIdx, mYstepIdx;
    double mFactorX, mFactorY;
    double mAspect;

    QList<double> mStepList;

    double mZoomXul, mZoomYul, mZoomXlr, mZoomYlr;
    bool mShowZoom;
    QColor mZoomColor;

    enum_draw_coord_num mAbscissa;
    enum_draw_coord_num mOrdinate;
};

#endif // OPENGL2D_MODEL_H
