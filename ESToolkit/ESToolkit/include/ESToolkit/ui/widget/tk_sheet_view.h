/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef SHEET_VIEW_H
#define SHEET_VIEW_H

#include <QESTkUtils/tile_engine/tk_source_view.h>
#include <ESToolkit/core/tk_asset_manager.h>
#include <libGyro/gyro_video_api.h>
#include <libGyro/gyro_texture_api.h>

class SheetView : public SourceView, public AssetManager  {
protected:
    GY_RECT_TEX_ORIENT  _orient     = GY_RECT_TEX_ORIENT_NORMAL;
    GY_RECT_PLANE_TYPE  _plane      = GY_RECT_PLANE_BOTTOM;
    unsigned            _elev       = 0;

public:
    explicit            SheetView(QWidget* parent=nullptr);

    void                setOrient(const GY_RECT_TEX_ORIENT orient);
    void                setPlane(const GY_RECT_PLANE_TYPE plane);
    void                setElev(const unsigned elev);

	//Implement external selection logic of SourceView
    virtual void        _externalSelectionCreate(void);
    virtual void        _externalSelectionClear(void);
};




// =================================
inline void SheetView::setOrient(const GY_RECT_TEX_ORIENT orient) { _orient = orient; }
inline void SheetView::setPlane(const GY_RECT_PLANE_TYPE plane) { _plane = plane; }
inline void SheetView::setElev(const unsigned elev) { _elev = elev; }


#endif
