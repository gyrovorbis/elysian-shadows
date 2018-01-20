#include <widget/tk_2d_vector_widget.h>
#include <QPainter>
#include <QMouseEvent>

Vector2DWidget::Vector2DWidget(QWidget* parent):
    QWidget(parent)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    setMinimumSize(100, 100);
    resize(200, 200);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

GYVector2 Vector2DWidget::_clipMagToRadius(GYVector2& vec, float* magPtr) const {
    GYVector2 out = vec;
    float mag = gyVec2Magnitude(&vec);
    if(mag > _radius) {
        if(magPtr) *magPtr = _radius;
        gyVec2ScaleAccum(&out, 1.0f/mag*(_radius));
    } else if(magPtr) *magPtr = mag;
    return out;
}

QPoint Vector2DWidget::_clipPointToCircle(const QPoint& pt) const {
    const GYVector2 centerV { (float)_center.x(), (float)_center.y() };
    GYVector2 vec = { (float)pt.x(), (float)pt.y() };
    GYVector2 diff;
    gyVec2Sub(&diff, &vec, &centerV);

    GYVector2 newDiff = _clipMagToRadius(diff);
    gyVec2Add(&vec, &centerV, &newDiff);

    return QPoint(vec.x, vec.y);
}

float Vector2DWidget::_angleFromCenter(const QPoint& pt) const {
    return atan2(pt.y() - _center.y(), pt.x() - _center.x());
}


void Vector2DWidget::paintEvent(QPaintEvent *event) {



    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(2);

    painter.setBrush(Qt::white);
    painter.setPen(pen);


    QRect r = rect();
        r.setSize(QSize(_minSideSize, _minSideSize));
       r.moveCenter(_center);

       painter.drawEllipse(r);

       pen.setWidth(1);
       pen.setColor(Qt::gray);
       painter.setPen(pen);
       painter.drawLine(_center-QPoint(_radius, 0), _center+QPoint(_radius, 0));
       painter.drawLine(_center-QPoint(0, _radius), _center+QPoint(0, _radius));

       if(_vectorMag != 0.0f) {
            pen.setColor(Qt::blue);
            pen.setWidth(2);
            painter.setPen(pen);
            QPoint endPt = _center + QPoint(_vector.x*_radius, _vector.y*_radius);
            painter.drawLine(_center, endPt);

            painter.save();
            painter.translate(endPt);
            painter.rotate(_vectorAngle*180.0f/M_PI);



            painter.drawLine(QPointF(0.0f, 0.0f), QPointF(-1.0f*_radius/10.0f, -1.0f*_radius/10.0f));
            painter.drawLine(QPointF(0.0f, 0.0f), QPointF(-1.0f*_radius/10.0f, 1.0f*_radius/10.0f));

            painter.restore();
       }


       if(_hovering) {
           painter.setBrush(QBrush(Qt::red));
           painter.setPen(Qt::NoPen);
           painter.drawEllipse(_mouseClippedPos, 2, 2);
       }



    //painter.translate(width() / 2, height() / 2);
    //painter.scale(radius / 200.0, radius / 200.0);
   // painter.drawEllipse(QPoint(0, 0), radius, radius);

    painter.end();
    QWidget::paintEvent(event);

}
void Vector2DWidget::mouseMoveEvent(QMouseEvent *event) {
    bool updateVec = false;
    _mousePos = event->pos();
    _mouseClippedPos = _clipPointToCircle(_mousePos);
    if(event->buttons() & Qt::LeftButton) {
        _mouseDownPos           = event->pos();
        _mouseDownClippedPos    = _clipPointToCircle(_mouseDownPos);
        _updateVectorInfo();
        updateVec = true;
    }

    update();
    QWidget::mouseMoveEvent(event);
}

void Vector2DWidget::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        if(!_mouseDown) {
            _mouseDown = true;
            Q_EMIT mouseLeftButtonPressed();
        }
    }
    QWidget::mousePressEvent(event);

}

void Vector2DWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        if(_mouseDown) {
            Q_EMIT mouseLeftButtonReleased();
            _mouseDown = false;
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void Vector2DWidget::resizeEvent(QResizeEvent* event) {
    _minSideSize = qMin(width(), height());
    _radius = _minSideSize/2.0f;
    _center = QPoint(width()/2.0f,height()/2.0f);
    QWidget::resizeEvent(event);
    QWidget::resizeEvent(event);
}

bool Vector2DWidget::event(QEvent *event) {
    switch(event->type()) {
    case QEvent::HoverEnter:
        _hovering = true;
        break;
    case QEvent::HoverLeave:
        _hovering = false;
        break;
    default: break;
    }

    return QWidget::event(event);
}

void Vector2DWidget::_updateVectorInfo(void) {
    _vectorAngle            = _angleFromCenter(_mouseDownClippedPos);

    GYVector2 center = { (float)_center.x(), (float)_center.y() };
    GYVector2 endPt  = { (float)_mouseDownClippedPos.x(), (float)_mouseDownClippedPos.y() };
    GYVector2 vec;

    gyVec2Sub(&vec, &endPt, &center);
    float newMag = gyVec2Magnitude(&vec)/_radius;
    gyVec2Normalize(&vec);
    gyVec2ScaleAccum(&vec, newMag);
    //Q_ASSERT(gyVec2Magnitude(&vec) == newMag);

    if(vec.x != _vector.x || vec.y != _vector.y) {
        _vector         = elysian::Vector2(vec.x, vec.y);
        _vectorMag      = newMag;
        Q_EMIT vectorChanged(_vector, newMag);

    }
}
