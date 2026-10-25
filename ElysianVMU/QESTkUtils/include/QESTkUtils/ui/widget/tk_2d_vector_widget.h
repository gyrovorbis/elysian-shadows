/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_2D_VECTOR_WIDGET_H
#define TK_2D_VECTOR_WIDGET_H

#include <QWidget>
#include <AssetIO/primitive/elysian_instance_transform.hpp>

//default colors and widths for everything
//default settings for rendering axes, etc
//default arrow scalar factor sizes

class Vector2DWidget: public QWidget {
    Q_OBJECT

public:
    explicit            Vector2DWidget(QWidget* parent=nullptr);


    virtual void        paintEvent(QPaintEvent *event) override;
    virtual void        mouseMoveEvent(QMouseEvent *event) override;
    virtual void        mousePressEvent(QMouseEvent* event) override;
    virtual void        mouseReleaseEvent(QMouseEvent* event) override;
    virtual void        resizeEvent(QResizeEvent *event) override;
    virtual bool        event(QEvent *event) override;

Q_SIGNALS:
    void                mouseLeftButtonPressed(void);
    void                mouseLeftButtonReleased(void);
    void                vectorChanged(elysian::Vector2 vec, float mag);


private:

    void                _updateVectorInfo(void);
    GYVector2           _clipMagToRadius(GYVector2& vec, float* mag=nullptr) const;
    QPoint              _clipPointToCircle(const QPoint& pt) const;
    float               _angleFromCenter(const QPoint& pt) const;

    elysian::Vector2    _vector;
    QPoint              _mousePos;
    QPoint              _mouseClippedPos;
    QPoint              _mouseDownPos;
    QPoint              _mouseDownClippedPos;
    QPoint              _center;
    float               _radius                 = 0.0f;
    float               _vectorAngle            = 0.0f;
    float               _vectorMag              = 0.0f;
    int                 _minSideSize            = 0;
    bool                _mouseDown              = false;
    bool                _hovering               = false;

};


#endif // TK_2D_VECTOR_WIDGET_H
