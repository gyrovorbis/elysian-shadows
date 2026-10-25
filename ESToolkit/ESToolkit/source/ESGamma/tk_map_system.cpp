/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/ESGamma/tk_map_system.h>

elysian::Material* TkMapSystem::_bgMat  = nullptr;
elysian::Material* TkMapSystem::_selMat = nullptr;
bool TkMapSystem::_renderMapBackground  = false;

void TkMapSystem::staticInitialize(void) {
    MapSystemBase::staticInitialize();

    _bgMat = new elysian::Material;
    _bgMat->setEmissiveColor(GYVector4{0.0f, 0.0f, 0.0f, 0.0f});

    _selMat = new elysian::Material[DRAW_MAX];
    _selMat[DRAW_SELECTING].setEmissiveColor(GYVector4{0.0f, 0.0f, 0.0f, 0.0f});
    _selMat[DRAW_SELECTION].setEmissiveColor(GYVector4{0.0f, 0.0f, 0.0f, 0.0f});
    _selMat[DRAW_FILLING].setEmissiveColor(GYVector4{0.0f, 0.0f, 0.0f, 0.0f});

}

void TkMapSystem::staticUninitialize(void) {
    MapSystemBase::staticUninitialize();

    delete      _bgMat;
    delete[]    _selMat;
}

void TkMapSystem::_renderMapBorders(void) const {
    if(!_renderMapBackground || getLightSys()->isShadowPass()) return;

    GYColor4 color = { 255, 255, 255, 100 };
    float xPos  =  (_scrStartX + _scrEndX)*16.0f;
    float yPos  =  (_scrStartY + _scrEndY)*16.0f;
    float xSize =  (_scrEndX - _scrStartX)*32.0f + 32.0f;
    float ySize =  (_scrEndY - _scrStartY)*32.0f + 32.0f;

    //if(xSize <= 0.0f || ySize <= 0.0f) return;

    GYVidPipelineState state = {
        0,
        nullptr,
        nullptr,
        nullptr,
        GY_ALPHA_FULL,
        GY_BLEND_SRC_ALPHA,
        GY_BLEND_ONE,
        0,
        GY_VID_DEPTH_ALWAYS,
        { 0, 0, 0, 0 }
    };

    gyVidMaterialBind(_bgMat);
    gyVidPipelineState(&state);

    gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
    gyVidQuadListBegin(&color);

    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslateScale(xPos, yPos, 1.0f, xSize, ySize, ySize);
    gyVidRect(0);
    //gyVidRect(0);
    gyMatPop();

    gyVidQuadListEnd();
    //gyVidDepthTest(false);
}
