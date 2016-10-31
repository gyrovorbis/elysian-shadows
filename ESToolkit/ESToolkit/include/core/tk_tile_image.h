#ifndef TILE_IMAGE_H
#define TILE_IMAGE_H

#include <QPixmap>
#include <QIcon>

class TileImage {
private:
	QPixmap _pixmap;
	QIcon *_icon;

public:
	//unsigned char id; //Why the FUCK?

	QPixmap getPixmap(void) const {
		return _pixmap;
	}

    void setPixmap(const QPixmap pixmap) {
		_pixmap = pixmap;
		delete _icon;
		_icon = new QIcon(_pixmap);
	}

	QIcon getIcon(void) const {
		return *_icon;
	}

	//void setPixmap(const QPixmap img) { image = img; }

	TileImage(const QPixmap pixmap): _icon(NULL) {
		_pixmap = pixmap;
		_icon = new QIcon(_pixmap);
	}

	TileImage(const QImage image): _icon(NULL) {
		_pixmap = QPixmap::fromImage(image);
		_icon = new QIcon(_pixmap);
	}

	~TileImage(void) { delete _icon; }

};

#endif
