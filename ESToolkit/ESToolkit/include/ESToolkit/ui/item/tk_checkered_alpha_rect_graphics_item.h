/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef CHECKERED_ALPHA_RECT_GRAPHICS_ITEM_H
#define CHECKERED_ALPHA_RECT_GRAPHICS_ITEM_H

#include <QGraphicsRectItem>
#include <QPainter>

#define CHECKERED_ALPHA_RECT_GRAPHICS_ITEM_IMAGE ":/misc/images/alphaChecker.png"

class CheckeredAlphaRectGraphicsItem: public QGraphicsRectItem {
private:
	QPixmap pixmap;
public:
	CheckeredAlphaRectGraphicsItem(void): pixmap(CHECKERED_ALPHA_RECT_GRAPHICS_ITEM_IMAGE) {}

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/) {
		//We need a little bit of reesty direct matrix manipulation here to extract the scale factor for later use, clear it from the painter,
		//and maintain the translation factor.
		const QTransform oldXForm = painter->transform();
		const qreal sx = oldXForm.m11();
		const qreal sy = oldXForm.m22();

		QTransform newXForm;
		newXForm.setMatrix(1.0f, oldXForm.m12(), oldXForm.m13(), oldXForm.m21(), 1.0f, oldXForm.m23(), oldXForm.m31(), oldXForm.m32(), oldXForm.m33());
		painter->setTransform(newXForm);

		//painter->setClipRect(option->exposedRect);
		painter->drawTiledPixmap(-16.0f*sx, -16.0f*sy, rect().width()*sx, rect().height()*sy, pixmap);

		painter->setTransform(oldXForm);
	}
};

#endif
