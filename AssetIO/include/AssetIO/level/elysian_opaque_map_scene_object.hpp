#ifndef ELYSIAN_OPAQUE_MAP_SCENE_OBJECT_HPP
#define ELYSIAN_OPAQUE_MAP_SCENE_OBJECT_HPP

#include <AssetIO/level/elysian_scene_object.hpp>
#include <ESGamma/system/elysian_compute_system.hpp>

#define ELYSIAN_RENDER_SYSTEM_POPULATE_MAP_IBO_KERNEL_NAME "PopulateMapIbo"
#define ELYSIAN_RENDER_SYSTEM_POPULATE_MAP_IBO_KERNEL_PATH "/assets/opencl/populate_map_ibo.cl"

namespace elysian {

#if 0
class LightCullingComputeTask: public ComputeTask {
public:

                    LightCullingComputeTask(uintptr_t depthBuffTexId,
                                            uintptr_t lightingUboId);

    void            setSampleDepthBuffer(bool val=true);
    bool            isSamplingDepthBuffer(void) const;
    bool            updateScreenResolution(unsigned int width, unsigned int height);

    bool            _sampleDepthBuffer      = false;
    uint32_t        _tileSize               = ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_TILE_SIZE_DEFAULT;
    uint32_t        _lightMapTexId           = 0;
    ComputeEvent    _completeEvent;

    ComputeBuffer*  _tileSizeBuffer         = nullptr;
    ComputeBuffer*  _depthBuffer            = nullptr;
    ComputeBuffer*  _lightMapBuff           = nullptr;
    ComputeBuffer*  _lightingUboBuff        = nullptr;
    ComputeBuffer*  _projViewMatBuff        = nullptr;
};

class LuminanceHistogramComputeTask: public ComputeTask {
public:

    struct LuminanceStats {
        uint32_t max;
        uint32_t min;
        uint32_t histBins[256];
        float    exp        = 1.0f;
        float    prevExp    = 1.0f;
    }               _lumStats;

    LuminanceHistogramComputeTask(uintptr_t textureId);

    ComputeEvent    _readCompleteEvent;
    ComputeBuffer*  _statsBuffer        = nullptr;
    ComputeBuffer*  _fboBuffer          = nullptr;
    float           _exposure           = 1.0f;
};

#endif


    class PopulateMapIboComputeTask: public ComputeTask, public RendererComputeGlobalDataManager {
    private:

        ComputeBuffer*  _drawIndirectBuffer     = nullptr;
        ComputeBuffer*  _mapIboBuffer           = nullptr;
        ComputeBuffer*  _mapVboBuffer           = nullptr;

        intptr_t        _glDrawIndirectBuffer   = 0;
        uint32_t        _width                  = 0;
        uint32_t        _height                 = 0;
        uint32_t        _tilesPerWorkItem       = 1;


    public:


        //onAreaChangeEvent() { swap VBO }

                        PopulateMapIboComputeTask(void);
                        ~PopulateMapIboComputeTask(void);

        void            updateArea(Area* area); //update immediately
        void            setTilesPerWorkItem(unsigned tiles);
        uintptr_t       getDrawIndirectBuffer(void) const;


        bool            populateIbo(void);
        bool            waitForIboFilled(void) const;

    };


    class OpaqueMapSceneObject: public SceneObject {
    public:

        virtual void render(void) override;

    };





    //========== INLINEZ ============

    inline uintptr_t PopulateMapIboComputeTask::getDrawIndirectBuffer(void) const { return _glDrawIndirectBuffer; }


}


#endif // ELYSIAN_OPAQUE_MAP_SCENE_OBJECT_HPP
