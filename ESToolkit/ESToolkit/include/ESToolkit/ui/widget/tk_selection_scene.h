/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef SELECTION_SCENE_H
#define SELECTION_SCENE_H

#include <QGraphicsScene>
#include <QEvent>

class SelectionScene: public QGraphicsScene {

	bool event(QEvent *event) {
		switch(event->type()) {
		//Causes the main scene to resort the entire map and SHIT ITS PANTS!
		case QEvent::WindowDeactivate:
			return true; //FUCK YOU, BITCH! INTERCEPTED!!!!!
		default:
			return QGraphicsScene::event(event);
		}
	}


};


#endif // MAPSCENE_H
