#ifndef TILE_SHEET_UTILITIES_H
#define TILE_SHEET_UTILITIES_H
//QT
#include <QString>
#include <QVector>
#include <QPixmap>
#include "gyro.h"

class TileImage;
class SpriteSheet;

class TileSheetUtilities {
public:
	static bool GetImageSize(const QString loc, unsigned int &width, unsigned int &height);
    static bool VerifyImageSize(const QString name, const unsigned int width, const unsigned int height);
	static bool DivideTileSheet(const QString loc, const unsigned int sheetWidth, const unsigned int sheetHeight, const unsigned int spriteWidth, const unsigned int spriteHeight, QVector<TileImage *> &container);
	static bool DivideTileSheet(const QString loc, const unsigned int sheetWidth, const unsigned int sheetHeight, const unsigned int spriteWidth, const unsigned int spriteHeight, QVector<QPixmap> &container);
	static bool GenerateTileSheet(const QString loc, const unsigned int sheetWidth, const unsigned int sheetHeight, const unsigned int spriteWidth, const unsigned int spriteHeight, QVector<TileImage *> &container);

	static bool DivideSpriteSheet(SpriteSheet &sheet);
};

#endif // TILEMANAGER_H
