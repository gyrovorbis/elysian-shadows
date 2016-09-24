#ifndef ELYSIAN_LIGHT_SYSTEM_H
#define ELYSIAN_LIGHT_SYSTEM_H

#include "elysian_system.hpp"
#include <crtp/elysian_area.hpp>
#include "elysian_subprocess_manager.hpp"
#include <crtp/elysian_light_source.hpp>
#include <gyro_matrix_api.h>

#define ELYSIAN_LIGHT_SYSTEM_MAX_LIGHTS_ONSCREEN		32
#define ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_WIDTH			ELYSIAN_AREA_MAP_WIDTH
#define ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_HEIGHT			ELYSIAN_AREA_MAP_HEIGHT
#define ELYSIAN_LIGHT_SYSTEM_INTERP_POOL_SIZE           10

namespace elysian {

    class LightSystem;
    typedef LinearInterpolatorSubProcess<LightAttrib, LightSystem> LightAttribInterp;
    typedef MonitorableSubProcessPool<LightAttribInterp, LightSystem, ELYSIAN_LIGHT_SYSTEM_INTERP_POOL_SIZE> LightAttribInterpPool;

    class LightSystem:
        public System,
        public SubProcessManager<LightSystem, LightAttribInterpPool>
    {
        enum SUBPROC_ID { SUBPROC_ATTRIB_INTERP_POOL };

        bool			_shadowPass         = false;
        bool            _shadowPassEnabled  = true;
        bool            _bloomEnabled       = true;
        bool			_enabled            = true;
        bool			_renderDebug        = false;
        unsigned		_onscreenLightCount	= 0;
        LightSource*	_onscreenLights[ELYSIAN_LIGHT_SYSTEM_MAX_LIGHTS_ONSCREEN] = { nullptr };
        uint32_t		_lightMap[ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_HEIGHT][ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_WIDTH] = { {0} };
        unsigned		_screenTileXOffset	= 0;
        unsigned		_screenTileYOffset	= 0;
        uint32_t		_activeLights		= 0;
        unsigned		_activeLightCount	= 0;

        unsigned		_shaderUniformChangesThisFrame = 0;
        unsigned		_shaderUniformChangesLastFrame = 0;

        void            _renderShadowVolumes(void);
        void            _addToLightMap(LightSource*const src);
        void            _sortOnscreenLights(void);

    public:

        void renderCube(float x, float y, float z,
                               float sx, float sy, float sz,
                               float /*rotz*/);

                    LightSystem(void);
        void        shadowRenderPasses(void);
        void        applyBloomEffect(void) const;
        unsigned    getLightCount(void) const;
        unsigned    getOnscreenLightCount(void) const;
        bool        isShadowPass(void) const;

        void        addLightSourcesToScene(void);
        void        removeLightSourcesFromScene(void);

        void        update(void);
        void        render(void);

        void        setGlobalAmbientColor(const GYVector4& color);
        void        getGlobalAmbientColor(GYVector4& color) const;

        void        setEnabled(const bool val);
        void        setShadowPassEnabled(const bool val);
        void        setBloomEnabled(const bool val);
        bool        getShadowPassEnabled(void) const;
        bool        isBloomEnabled(void) const;

        void        setRenderDebugEnabled(const bool val);

        //uint32_t getLightMapEntryScreenCoord(const unsigned y, const unsigned x) const;
        uint32_t    getLightMapEntryMapCoord(const unsigned y, const unsigned x) const;
        uint32_t    getLightMask(const Entity& entity) const;

        void        getLightPassTileRegion(int& startX, int& startY, int& endX, int& endY) const;
        uint32_t    startLightPass(uint32_t& lightsLeft);

        unsigned    getShaderUniformChangesLastFrame(void) const;

        void        interpLightAttrib(const float duration, const LightAttrib &newAttrib, LightAttrib*const attrib, EventMonitor*const monitor=nullptr);
    };









    // ==== INLINEZ ======



    inline void LightSystem::setRenderDebugEnabled(const bool val) {
        _renderDebug = val;
    }
    /*
    inline uint32_t LightSystem::getLightMapEntryScreenCoord(const unsigned y, const unsigned x) const {
        return _lightMap[y][x];
    }*/

    inline uint32_t LightSystem::getLightMapEntryMapCoord(const unsigned y, const unsigned x) const {
        if(y >= _curArea->height || x >= _curArea->width) {
            return 0;
        }
        return (_shadowPass || !_enabled)? 0 :_lightMap[y-_screenTileYOffset][x-_screenTileXOffset];
    }

    inline unsigned LightSystem::getOnscreenLightCount(void) const {
        return (_shadowPass)? 0 : _onscreenLightCount;
    }

    inline unsigned LightSystem::getShaderUniformChangesLastFrame(void) const {
        return _shaderUniformChangesLastFrame;
    }

    inline bool LightSystem::isShadowPass(void) const { return _shadowPass; }

    inline void LightSystem::interpLightAttrib(const float duration, const LightAttrib &newAttrib, LightAttrib*const attrib, EventMonitor*const monitor) {
        auto& proc = get<SUBPROC_ATTRIB_INTERP_POOL> ( static_cast<SubProcessManager<LightSystem,
                                                  LightAttribInterpPool>& >(*this));

        auto& subProc = proc.claimIdleSubProcess();
        subProc.setKeyFrames(attrib, newAttrib);
        subProc.setTimeInterval(duration);
        subProc.setEventMonitor(monitor);
        subProc.start(this);
    }

    inline void LightSystem::getGlobalAmbientColor(GYVector4& color) const {
        color = _curArea->_ambientColor;
    }

    inline void LightSystem::setShadowPassEnabled(const bool val) {
        _shadowPassEnabled = val;
    }

    inline void LightSystem::setBloomEnabled(const bool val) {
        _bloomEnabled = val;
    }

    inline bool LightSystem::isBloomEnabled(void) const {
        return _bloomEnabled;
    }

    inline bool LightSystem::getShadowPassEnabled(void) const {
        return _shadowPassEnabled;
    }

    inline uint32_t LightSystem::getLightMask(const Entity& /*entity*/) const {
         if(_shadowPass || !_enabled) return 0;
        uint32_t mask = 0;
        for(unsigned i = 0; i < getOnscreenLightCount(); ++i) mask |= (0x1<<i);
        return mask;
    }
}

#endif // ELYSIAN_LIGHT_SYSTEM_H
