#ifndef TK_PARTICLE_SYSTEM_H
#define TK_PARTICLE_SYSTEM_H

#include <system/elysian_particle_system_base.hpp>

#define TK_PARTICLE_SYSTEM_PREVIEW_POS_DEFAULT          elysian::Vector3(300.0f, 300.0f, 100.0f)
#define TK_PARTICLE_SYSTEM_PREVIEW_DIR_DEFAULT          elysian::Vector3(300.0f, 300.0f, 100.0f)
#define TK_PARTICLE_SYSTEM_PREVIEW_INTENSITY_DEFAULT    1000
#define TK_PARTICLE_SYSTEM_PREVIEW_SPREAD_DEFAULT       10.0f

class ParticleAttribPreviewDockWidget;

class TkParticleSystem: public elysian::ParticleSystemBase {
public:
    struct PreviewProperties {
        elysian::Vector3    pos         = TK_PARTICLE_SYSTEM_PREVIEW_POS_DEFAULT;
        elysian::Vector3    dir         = TK_PARTICLE_SYSTEM_PREVIEW_DIR_DEFAULT;
        unsigned            intensity   = TK_PARTICLE_SYSTEM_PREVIEW_INTENSITY_DEFAULT;
        float               spreadAngle = TK_PARTICLE_SYSTEM_PREVIEW_SPREAD_DEFAULT;

        elysian::Vector3&   getPos(void);
        elysian::Vector3&   getDir(void);
        unsigned            getIntensity(void) const;
        void                setIntensity(unsigned val);
        float               getSpreadAngle(void) const;
        void                setSpreadAngle(float degrees);
    };

                            TkParticleSystem(void);
    virtual                 ~TkParticleSystem(void);
    PreviewProperties&      getPreviewProperties(void);
    unsigned                getPreviewPoolSize(void) const;
    void                    resizePreviewPool(unsigned size);
    ParticleEmitterAttrib*  getPreviewAttrib(void) const;
    void                    setPreviewAttrib(ParticleEmitterAttrib* attrib);
    unsigned                getPreviewEmitterIndex(void) const;
    unsigned                getPreviewActiveParticleCount(void) const;

    void                    emitPreview(ParticleEmitterAttrib* attrib=nullptr);
    void                    popDockWidget(ParticleEmitterAttrib* attrib=nullptr);
    auto                    getStdItems(void) -> std::pair<QStandardItem*, QStandardItem*>;

private:

    void                    _createItems(void);

    PreviewProperties                   _previewProperties;
    QStandardItem*                      _labelItem          = nullptr;
    ESTkStandardItem*                   _parentItem         = nullptr;
    PropertyStackModel*                 _model              = nullptr;
    ParticleAttribPreviewDockWidget*    _previewWidget      = nullptr;
    const unsigned                      _tkEmitterIndex;

};




//========= INLINEZ =========

inline elysian::Vector3& TkParticleSystem::PreviewProperties::getPos(void) { return pos; }
inline elysian::Vector3& TkParticleSystem::PreviewProperties::getDir(void) { return dir; }
inline unsigned          TkParticleSystem::PreviewProperties::getIntensity(void) const { return intensity; }
inline void              TkParticleSystem::PreviewProperties::setIntensity(unsigned val) { intensity = val; }
inline float             TkParticleSystem::PreviewProperties::getSpreadAngle(void) const { return spreadAngle; }
inline void              TkParticleSystem::PreviewProperties::setSpreadAngle(float degrees) { spreadAngle = degrees; }

inline auto              TkParticleSystem::getPreviewProperties(void) -> PreviewProperties& { return _previewProperties; }
inline ParticleEmitterAttrib* TkParticleSystem::getPreviewAttrib(void) const { return getAttrib(_tkEmitterIndex); }
inline void TkParticleSystem::setPreviewAttrib(ParticleEmitterAttrib* attrib) { setAttrib(_tkEmitterIndex, attrib); }
inline unsigned TkParticleSystem::getPreviewEmitterIndex(void) const { return _tkEmitterIndex; }




#endif // TK_PARTICLE_SYSTEM_HPP
