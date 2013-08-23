#ifndef SELECTIONITEMGROUP_H
#define SELECTIONITEMGROUP_H

#include <QGraphicsItemGroup>
#include <QPainterPath>
#include <QDebug>


class SelectionItemGroup: public QGraphicsItemGroup {
public:
	static inline QPointF clampToTileLoc(const QPointF &point) {
		return QPointF((((int(point.x()+16)/32))*32)-16,
					   (((int(point.y()+16)/32))*32)-16);
	}

	inline virtual QPainterPath opaqueArea(void) { return path; }
	inline SelectionItemGroup(QGraphicsItem * parent = 0): QGraphicsItemGroup(parent) { setFlag(ItemSendsGeometryChanges, true); }
	inline virtual void paint (QPainter */*painter*/, const QStyleOptionGraphicsItem * /*option*/, QWidget */*widget = 0*/) { }

	inline virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) {
		if(change == ItemPositionChange && scene()) {
			QPointF newPos = clampToTileLoc(value.toPointF());

			//Logic for forcing selection region to stay in the scene
			/*
			QRectF rect = scene()->sceneRect();
			qDebug() << rect;
			if (!rect.contains(newPos)) {
				if(newPos.x() < rect.left()) newPos.setX(rect.left());
				else if(newPos.x() > rect.right()) newPos.setX(rect.right());

				if(newPos.y() < rect.top()) newPos.setY(rect.top());
				else if(newPos.y() > rect.bottom()) newPos.setY(rect.bottom());

				//newPos.setX(qMin(rect.right()-16, qMax(newPos.x(), rect.left()-16)));
				//newPos.setY(qMin(rect.bottom()-16, qMax(newPos.y(), rect.top()-16)));
			}*/
			return newPos;
		}
		return QGraphicsItem::itemChange(change, value);
	}

    QPainterPath path;

};

#endif // SELECTIONITEMGROUP_H
