// --------------------------------------------------------------------------------
/*
    draw2d_model.cpp

  Copyright (c) 2009  Thomas Mueller, Frank Grave


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
// -------------------------------------------------------------------------------//

#include "draw2d_model.h"

#include <QHBoxLayout>

/*! Standard constructor.
 */
Draw2dModel::Draw2dModel() {
    plot = new QwtPlot;
    QwtPlotGrid* grid = new QwtPlotGrid;
    grid->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));
    grid->attach(plot);

    mButtonPressed = Qt::NoButton;
    setFocusPolicy(Qt::ClickFocus);
    mKeyPressed = Qt::Key_No;

    setMinimumSize(DEF_DRAW2D_WIDTH, DEF_OPENGL_HEIGHT);
    setMaximumSize(DEF_DRAW2D_WIDTH, DEF_OPENGL_HEIGHT);

    mCurrColor = Qt::white;

    scaleX = QPointF(DEF_DRAW2D_SCALE_X_INIT_MIN, DEF_DRAW2D_SCALE_X_INIT_MAX);
    scaleY = QPointF(DEF_DRAW2D_SCALE_Y_INIT_MIN, DEF_DRAW2D_SCALE_Y_INIT_MAX);

    double factor = double(DEF_DRAW2D_WIDTH) / double(DEF_OPENGL_HEIGHT);
    xLabel = ceil((scaleX.y() - scaleX.x()) / 20.0);
    yLabel = ceil((scaleY.y() - scaleY.x()) / 20.0);
    plot->setAxisScale(QwtPlot::xBottom, scaleX.x()*factor, scaleX.y()*factor, xLabel);
    plot->setAxisScale(QwtPlot::yLeft, scaleY.x(), scaleY.y(), yLabel);

    curve = new QwtPlotCurve();
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setPen(QPen(Qt::blue));
    curve->setStyle(QwtPlotCurve::Lines);
    curve->attach(plot);

    mAbscissa = mOrdinate = 0;

    if (!mObjects.empty()) {
        mObjects.clear();
    }

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(plot);
    this->setLayout(layout);
}

/*! Standard destructor.
 */
Draw2dModel::~Draw2dModel() {
}

// ************************************ public methods ********************************

/*! Set line style.
 * \param style : style of curve (lines or dots).
 */
void Draw2dModel::setStyle(const enum_curve_style  style) {
    switch (style) {
        case enum_curve_lines:
            curve->setStyle(QwtPlotCurve::Lines);
            break;
        case enum_curve_dots:
            curve->setStyle(QwtPlotCurve::Dots);
            break;
    }
    plot->replot();
}

/*! Set points.
 * \param currMetric : pointer to current metric.
 * \param points : reference to points.
 * \param dtype : drawing type.
 */
void Draw2dModel::setPoints(m4d::Object* data, m4d::enum_draw_type  dtype) {
    assert(data != NULL);
    if (!mX.empty()) {
        mX.clear();
    }

    if (!mY.empty()) {
        mY.clear();
    }

    for (unsigned int i = 0; i < data->points.size(); i++) {
        m4d::vec4 tp;
        switch (dtype) {
            case m4d::enum_draw_pseudocart: {
                data->currMetric->transToPseudoCart(data->points[i], tp);
                mX.push_back(tp[1]);
                mY.push_back(tp[2]);
                break;
            }
            case m4d::enum_draw_coordinates: {
                mX.push_back(data->points[i][mAbscissa]);
                mY.push_back(data->points[i][mOrdinate]);
                break;
            }
            case m4d::enum_draw_embedding:
                break;
            case m4d::enum_draw_twoplusone:
                break;
            case m4d::enum_draw_custom: {
                data->currMetric->transToCustom(data->points[i], tp);
                mX.push_back(tp[1]);
                mY.push_back(tp[2]);
                break;
            }
        }
    }


    if (!mX.empty() && !mY.empty()) {
        curve->setData(mX, mY);
    }
    plot->replot();
}

/*! Set coordinate indices.
 * \param abscissa : coordinate index for the abscissa.
 * \param ordinate : coordinate index for the ordinate.
 */
void Draw2dModel::setCoordIndices(int abscissa, int ordinate) {
    mAbscissa = abscissa;
    mOrdinate = ordinate;

    if (mAbscissa < 0) {
        mAbscissa = 0;
    }
    if (mAbscissa > 3) {
        mAbscissa = 3;
    }
    if (mOrdinate < 0) {
        mOrdinate = 0;
    }
    if (mOrdinate > 3) {
        mOrdinate = 3;
    }
}

/*! Get coordinate indices.
 * \param abscissa : reference to coordinate index for the abscissa.
 * \param ordinate : reference to coordinate index for the ordinate.
 */
void Draw2dModel::getCoordIndices(int &abscissa, int &ordinate) {
    abscissa = mAbscissa;
    ordinate = mOrdinate;
}

/*! Set scaling.
 * \param sx : scaling point for x-direction.
 * \param sy : scaling point for y-direction.
 */
void Draw2dModel::setScaling(QPointF sx, QPointF sy) {
    scaleX = sx;
    scaleY = sy;
    double factor = double(DEF_DRAW2D_WIDTH) / double(DEF_OPENGL_HEIGHT);
    xLabel = ceil((scaleX.y() - scaleX.x()) / 20.0);
    yLabel = ceil((scaleY.y() - scaleY.x()) / 20.0);
    plot->setAxisScale(QwtPlot::xBottom, scaleX.x()*factor, scaleX.y()*factor, xLabel);
    plot->setAxisScale(QwtPlot::yLeft, scaleY.x(), scaleY.y(), yLabel);
    plot->replot();
}

/*! Get scaling.
 * \param sx : reference to scaling point for x-direction.
 * \param sy : reference to scaling point for y-direction.
 */
void Draw2dModel::getScaling(QPointF &sx, QPointF &sy) {
    sx = scaleX;
    sy = scaleY;
}

/*! Clear all objects.
 */
void Draw2dModel::clearObjects() {
    for (int i = 0; i < mObjects.size(); i++) {
        delete mObjects[i];
    }

    mObjects.clear();
    plot->replot();
}

/*! Add object.
 * \param points : list of points.
 * \param col : color of object.
 */
void Draw2dModel::addObject(QList<QPointF> &points, QColor col) {
    QwtPlotCurve* nc = new QwtPlotCurve();
    nc->setRenderHint(QwtPlotItem::RenderAntialiased);
    nc->setPen(QPen(col));
    nc->setStyle(QwtPlotCurve::Lines);
    nc->attach(plot);

    QwtArray<double> mX;
    QwtArray<double> mY;
    for (int i = 0; i < points.size(); i++) {
        mX.push_back(points[i].x());
        mY.push_back(points[i].y());
    }
    nc->setData(mX, mY);

    mObjects.push_back(nc);
    plot->replot();
}

// *********************************** protected methods ******************************

/*!
 *  \param event : key event.
 */
void Draw2dModel::keyPressEvent(QKeyEvent* event) {
    mKeyPressed = event->key();

}

/*!
 *  \param event : key event.
 */
void Draw2dModel::keyReleaseEvent(QKeyEvent* event) {
    mKeyPressed = Qt::Key_No;
    event->ignore();
}

/*!
 *  \param event : mouse event.
 */
void Draw2dModel::mousePressEvent(QMouseEvent * event) {
    mButtonPressed = event->button();
    mLastPos = event->pos();

    oldScaleX = scaleX;
    oldScaleY = scaleY;

}

/*!
 *  \param event : mouse event.
 */
void Draw2dModel::mouseReleaseEvent(QMouseEvent * event) {
    mButtonPressed = Qt::NoButton;
    event->accept();
}

/*!
 *  \param event : mouse event.
 */
void Draw2dModel::mouseMoveEvent(QMouseEvent * event) {
    QPoint mouse_pos = event->pos();

    if (mButtonPressed == Qt::LeftButton) {
        QPoint dx = mouse_pos - mLastPos;
        scaleX = oldScaleX - QPointF(0.03 * dx.x(), 0.03 * dx.x());
        scaleY = oldScaleY + QPointF(0.03 * dx.y(), 0.03 * dx.y());
        emit scalingChanged();
    } else if (mButtonPressed == Qt::MidButton) {
        QPoint dx = mouse_pos - mLastPos;
        double dr = 0.03 * dx.y();
        scaleX = oldScaleX + QPointF(dr, -dr);
        scaleY = oldScaleY + QPointF(dr, -dr);
        emit scalingChanged();

    }

    double factor = double(DEF_DRAW2D_WIDTH) / double(DEF_OPENGL_HEIGHT);
    xLabel = ceil((scaleX.y() - scaleX.x()) / 20.0);
    yLabel = ceil((scaleY.y() - scaleY.x()) / 20.0);
    plot->setAxisScale(QwtPlot::xBottom, scaleX.x()*factor, scaleX.y()*factor, xLabel);
    plot->setAxisScale(QwtPlot::yLeft, scaleY.x(), scaleY.y(), yLabel);

    plot->replot();
}
