/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/core/tk_tile_sheet_utilities.h>
#include <ESToolkit/ui/tk_estoolkit.h>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <ESToolkit/core/tk_tile_image.h>
#include <QESTkUtils/tile_engine/tk_tile_graphics_item.h>

#include <libGyro/gyro.h>
#include <cassert>
#include <QImageReader>
#include <QPainter>


bool TileSheetUtilities::VerifyImageSize(const QString loc, const unsigned int width, const unsigned int height) {
    QImageReader reader;

    reader.setFileName(loc);
    if(!reader.canRead()) {
        return false;
    }
    QSize size=reader.size();
	if((unsigned)size.width() != width) {
        return false;
    }
	if((unsigned)size.height() != height) { // % 32
        return false;
    }
    return true;
}

bool TileSheetUtilities::GetImageSize(const QString loc, unsigned int &width, unsigned int &height) {
	QImageReader reader;

	reader.setFileName(loc);
	if(!reader.canRead()) {
		Debug::Logf(Debug::LOG_WARNING, "TileSheetUtilities::GetImageSize(%s) - Could not open image!", loc.toStdString().c_str());
		return false;
	}
	QSize size=reader.size();
	width = size.width();
	height = size.height();
	return true;
}

bool TileSheetUtilities::GenerateTileSheet(const QString loc, const unsigned int sheetWidth, const unsigned int sheetHeight, const unsigned int spriteWidth, const unsigned int spriteHeight, QVector<TileImage *> &container) {
    QImage sheet(sheetWidth, sheetHeight, QImage::Format_ARGB32);
    QPainter *painter = new QPainter(&sheet);
    QColor fillColor(255, 255, 255, 0);

    sheet.fill(fillColor.rgba());

    unsigned int x = 0;
    unsigned int y = 0;
	for(int i = 0; i < container.size(); ++i) {
		painter->drawPixmap(x, y, container[i]->getPixmap());
		//TileGraphicsItem::renderFlipRotPixmap(painter, container[i]->getPixmap(), (orient)? orient[i], GY_RECT_TEX_ORIENT_NORMAL);
        x += spriteWidth;
        if(x >= sheetWidth) {
            y += spriteHeight;
            x = 0;
        }
        if(y >= sheetHeight) break;
    }

    delete painter;
    return sheet.save(loc);
}


bool TileSheetUtilities::DivideTileSheet(const QString loc, const unsigned int sheetWidth, const unsigned int sheetHeight, const unsigned int spriteWidth, const unsigned int spriteHeight, QVector<QPixmap> &container){

#if 0
	QImageReader reader;
    reader.setFileName(loc);
    QRect clipRect(0,0,spriteWidth,spriteHeight);
    int id = 0;
    if(!reader.canRead()) {
		Debug::Logf(Debug::LOG_WARNING, "Could not read image file %s!", loc.toStdString().c_str());
        return false;
    }


	for(; clipRect.y() < (int)sheetHeight; clipRect.translate (0,spriteHeight)) {
		for(clipRect.moveTo(0,clipRect.y()); clipRect.x() < (int)sheetWidth; clipRect.translate(spriteWidth,0)) {
            reader.setFileName(loc);
            reader.setClipRect(clipRect);
            QImage image = reader.read();

            if(image.isNull()) {
                QString error = reader.errorString();
				Debug::Log(Debug::LOG_WARNING, "TileManager::DivideTileSheet(): There was an error reading the image! Error message: " + error);
               return false;
            }

            VisualTile *tile=new VisualTile;
            tile->image=QPixmap::fromImage(image);
            tile->id=id++;
            container.push_back(tile);
        }
    }
    return true;
#else //HELL FUCKING YES!!!! Literally over 100x faster!
	QImage image(loc);
	QRect clipRect(0,0,spriteWidth,spriteHeight);
	//int id = 0;

	if(image.isNull()) {
		Debug::Logf(Debug::LOG_WARNING, "Could not read image file %s!", loc.toStdString().c_str());
		return false;
	}

	for(; clipRect.y() < (int)sheetHeight; clipRect.translate (0,spriteHeight)) {
		for(clipRect.moveTo(0,clipRect.y()); clipRect.x() < (int)sheetWidth; clipRect.translate(spriteWidth,0)) {
			QPixmap pixmap = QPixmap::fromImage(image.copy(clipRect));
			//TerrainImage *tile=new TerrainImage(image.copy(clipRect));
			//tile->id=id++;

			if(pixmap.isNull()) {
				Debug::Log(Debug::LOG_WARNING, "TileManager::DivideTileSheet(): There was an error reading the image!");
				//delete tile;
				return false;
			}

			container.push_back(pixmap);
		}
	}
	return true;
#endif
}

bool TileSheetUtilities::DivideTileSheet(const QString loc, const unsigned int sheetWidth, const unsigned int sheetHeight, const unsigned int spriteWidth, const unsigned int spriteHeight, QVector<TileImage*> &container) {
	QVector<QPixmap> pixmaps;

	if(DivideTileSheet(loc, sheetWidth, sheetHeight, spriteWidth, spriteHeight, pixmaps)) {
		for(int i = 0; i < pixmaps.size(); ++i) {
			container.push_back(new TileImage(pixmaps[i]));
		}
		return true;
	}
	return false;
}


/*
  Gracefully handles the sheet size not being a multiple of the sprite sizes by allowing for odd-sized border sprites.
  Note that if your rendering does not check for the QPixmap size before rendering, the borders will be distored when
  they are different sizes.
  */
bool TileSheetUtilities::DivideSpriteSheet(SpriteSheet &/*sprite*/) {
//Reested to shit from Renderable paradigm shift!
#if 0
	assert(sprite._attrib);
	QImage *img = new QImage(QString(sprite._attrib->getTexPath()));
    if(img->isNull()) {
		Debug::Logf(Debug::LOG_WARNING, "Could not open image filez %s!", sprite._attrib->getTexPath());
        return false;
    }

	int sheetWidth = img->width();
	int sheetHeight = img->height();
	unsigned spriteWidth, spriteHeight;
	sprite._attrib->getSpriteSize(spriteWidth, spriteHeight);

    delete img;

    QImageReader reader;
	reader.setFileName(QString(sprite._attrib->getTexPath()));
	QRect clipRect(0,0,spriteWidth,spriteHeight);

    if(!reader.canRead()) {
		Debug::Logf(Debug::LOG_WARNING, "Could not read image file %s!", sprite._attrib->getTexPath());
		//sprite._spriteAmount = 0;
        return false;
    }

	if((unsigned)sheetWidth < spriteWidth || (unsigned)sheetHeight < spriteHeight) return false;

	unsigned spriteAmount = (sheetWidth/spriteWidth)*(sheetHeight/spriteHeight);
	sprite._frames = new QPixmap[spriteAmount];

    unsigned int curRow = 0;
    unsigned int curColumn = 0;
	unsigned int rowSize = sheetWidth/spriteWidth;

	for(; clipRect.y() < sheetHeight; clipRect.translate (0,(clipRect.y() + (int)spriteHeight <= sheetHeight)? spriteHeight : sheetHeight-clipRect.y())) {
		for(clipRect.moveTo(0,clipRect.y()); clipRect.x() < sheetWidth; clipRect.translate((clipRect.x() + (int)spriteWidth <= sheetWidth)? spriteWidth : sheetWidth-clipRect.x(),0)) {
			reader.setFileName(QString(sprite._attrib->getTexPath()));
            reader.setClipRect(clipRect);
            QImage image = reader.read();

            if(image.isNull()) {
                QString error = reader.errorString();
                Debug::Log(Debug::LOG_WARNING, "TileManager::DivideSpriteSheet(): There was an error reading the image! Error message: " + error);
				//sprite._spriteAmount = 0;
                return false;
            }

			if(curRow*rowSize+curColumn >= spriteAmount) return false;

			sprite._frames[curRow*rowSize+curColumn] = QPixmap::fromImage(image);

            ++curColumn;
        }
        curColumn = 0;
        ++curRow;
    }
    return true;
#endif
	return false;
}
