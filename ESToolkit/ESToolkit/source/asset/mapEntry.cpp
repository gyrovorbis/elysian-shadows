#include <QPainter>
#include "mapEntry.h"
#include "elysian_level.hpp"

bool TileEntry::renderIndeces	= false;
bool TileEntry::renderFlipRot	= false;
bool TileEntry::renderElev		= false;
bool TileEntry::renderColliders = false;
bool TileEntry::renderZTran		= false;
bool TileEntry::renderPlane    = false;

void TileEntry::renderCollider(QPainter *const painter, const elysian::Tile::COLLIDER_TYPE collider) {
	static const QColor fillColor(0, 0, 255, 80);
	static const QPointF rTriSE[3] = {
		 QPointF(16.0f, -16.0f),
		 QPointF(16.0f, 16.0f),
		 QPointF(-16.0f, 16.0f)
	 };
	static const QPointF rTriNE[3] = {
		 QPointF(-16.0f, -16.0f),
		 QPointF(16.0f, -16.0f),
		 QPointF(16.0f, 16.0f)
	 };
	static const QPointF rTriNW[3] = {
		 QPointF(16.0f, -16.0f),
		 QPointF(-16.0f, -16.0f),
		 QPointF(-16.0f, 16.0f)
	 };
	static const QPointF rTriSW[3] = {
		 QPointF(-16.0f, -16.0f),
		 QPointF(-16.0f, 16.0),
		 QPointF(16.0f, 16.0f)
	 };

	//_pushTileFlipRotXForm(painter, orient);

	switch(collider) {
	case elysian::Tile::COLLIDER_NONE:
		break;
	case elysian::Tile::COLLIDER_FULL:
		painter->fillRect(QRect(-16, -16, 32, 32), fillColor);
		break;
	case elysian::Tile::COLLIDER_HALF_E:
		painter->fillRect(QRect(0, -16, 16, 32), fillColor);
		break;
	case elysian::Tile::COLLIDER_HALF_N:
		painter->fillRect(QRect(-16, -16, 32, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_HALF_W:
		painter->fillRect(QRect(-16, -16, 16, 32), fillColor);
		break;
	case elysian::Tile::COLLIDER_HALF_S:
		painter->fillRect(QRect(-16, 0, 32, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_QUAD_SE:
		painter->fillRect(QRect(0, 0, 16, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_QUAD_NE:
		painter->fillRect(QRect(0, -16, 16, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_QUAD_NW:
		painter->fillRect(QRect(-16, -16, 16, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_QUAD_SW:
		painter->fillRect(QRect(-16, 0, 16, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_L_SE:
		painter->fillRect(QRect(0, -16, 16, 16), fillColor);
		painter->fillRect(QRect(-16, 0, 32, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_L_NE:
		painter->fillRect(QRect(0, 0, 16, 16), fillColor);
		painter->fillRect(QRect(-16, -16, 32, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_L_NW:
		painter->fillRect(QRect(-16, 0, 16, 16), fillColor);
		painter->fillRect(QRect(-16, -16, 32, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_L_SW:
		painter->fillRect(QRect(-16, -16, 16, 16), fillColor);
		painter->fillRect(QRect(-16, 0, 32, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_QUADS_SE_NW:
		painter->fillRect(QRect(0, 0, 16, 16), fillColor);
		painter->fillRect(QRect(-16, -16, 16, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_QUADS_SW_NE:
		painter->fillRect(QRect(0, -16, 16, 16), fillColor);
		painter->fillRect(QRect(-16, 0, 16, 16), fillColor);
		break;
	case elysian::Tile::COLLIDER_45DEG_SE:
		painter->setPen(Qt::NoPen);
		painter->setBrush(QBrush(fillColor, Qt::SolidPattern));
		painter->drawConvexPolygon(rTriSE, 3);
		break;
	case elysian::Tile::COLLIDER_45DEG_NE:
		painter->setPen(Qt::NoPen);
		painter->setBrush(QBrush(fillColor, Qt::SolidPattern));
		painter->drawConvexPolygon(rTriNE, 3);
		break;
	case elysian::Tile::COLLIDER_45DEG_NW:

		painter->setPen(Qt::NoPen);
		painter->setBrush(QBrush(fillColor, Qt::SolidPattern));
		painter->drawConvexPolygon(rTriNW, 3);
		break;
	case elysian::Tile::COLLIDER_45DEG_SW:

		painter->setPen(Qt::NoPen);
		painter->setBrush(QBrush(fillColor, Qt::SolidPattern));
		painter->drawConvexPolygon(rTriSW, 3);
		break;
	default:
		Q_ASSERT(0);
	}

	//_popTileFlipRotXForm(painter, orient);
}


//====NONSTATIC====

void TileEntry::renderDecoration(QPainter *const painter, const unsigned /*x*/, const unsigned y) const {
	static const QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	static const QRectF rect(-16.0f, -16.0f, 32.99f, 32.99f);

	painter->setPen(pen);
	//Tile attrib
	if(renderColliders) {
		elysian::Tile::COLLIDER_TYPE collider = elysian::Tile::reorientCollider(getCurrentLevel()->getElysianTile(getIndex())->colliderType,
																				getOrient());
		renderCollider(painter, collider);
	}

	//TileEntry attrib
	if(renderZTran && getZTran()) {
		QBrush brush(Qt::DiagCrossPattern);
		brush.setColor(Qt::yellow);
		painter->fillRect(QRect(-16, -16, 32, 32), brush);
	}

    if(renderPlane) {
        GY_RECT_PLANE_TYPE plane = getPlane();

        if(plane != GY_RECT_PLANE_BOTTOM) {
            QString str;

            switch(plane) {
                case GY_RECT_PLANE_FRONT:
                    str = QString("F");
                    break;
                case GY_RECT_PLANE_LEFT:
                    str = QString("L");
                    break;
                case GY_RECT_PLANE_RIGHT:
                    str = QString("R");
                    break;
                case GY_RECT_PLANE_FRONT_SLOPE_45:
                    str = QString("F45");
                    break;
                case GY_RECT_PLANE_LEFT_SLOPE_45:
                    str = QString("L45");
                    break;
                case GY_RECT_PLANE_RIGHT_SLOPE_45:
                    str = QString("R45");
                    break;
                default:
                    Q_ASSERT(false);
            }
            painter->drawText(rect, Qt::AlignCenter, str);

            QColor color(Qt::white);
            color.setAlpha(100);
            painter->fillRect(rect, color);
        }
    }

	//MapEntry attrib
	if(renderIndeces) {
		unsigned index = getIndex();
		if(!index) return;
		painter->drawText(rect, Qt::AlignCenter, QString::number(index));

	} else if(renderFlipRot) {
		GY_RECT_TEX_ORIENT orient = getOrient();
		QString str;

		switch(orient) {
			case GY_RECT_TEX_ORIENT_NORMAL: return;
			case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
				str = QString("H");
				break;
			case GY_RECT_TEX_ORIENT_FLIP_VERT:
				str = QString("V");
				break;
			case GY_RECT_TEX_ORIENT_ROT_90:
				str = QString("90");
				break;
			case GY_RECT_TEX_ORIENT_ROT_180:
				str = QString("180");
				break;
			case GY_RECT_TEX_ORIENT_ROT_270:
				str = QString("270");
				break;
			case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
				str = QString("H90");
				break;
			case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
				str = QString("H270");
				break;
			default:
				Q_ASSERT(false);
		}
		painter->drawText(rect, Qt::AlignCenter, str);

	} else if(renderElev) {
		unsigned elev = getElev();
		if(!elev) return;
		unsigned yBase = y + elev;
		unsigned colorIndex = (yBase%16)+2;						//Use Qt's built in global colors, because I'm too lazy to make my own LUT.
		if(colorIndex > 3 && colorIndex < 7) colorIndex += 4;  //Bump it off the lame ass, repetitive color region.
		QColor color((Qt::GlobalColor)colorIndex);
		/* Quick hack to fix conflict between collision and elevation layer colours. @todo Make this more efficent. */
		if(color.blue() > 90) {
			color.setBlue(90);
		}
		color.setAlpha(100);
		painter->fillRect(rect, color);
		painter->drawText(rect, Qt::AlignCenter, QString::number(elev));

    }
}
