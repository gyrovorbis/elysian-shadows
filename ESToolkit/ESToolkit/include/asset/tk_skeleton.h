#ifndef TKSKELETON_H
#define TKSKELETON_H

#include <component/elysian_skeleton_base.hpp>

class TkSkeletonAttrib: public elysian::SkeletonAttribBase {
    Q_OBJECT
public:
    void    setJsonPath(const char*const path);
    void    setAtlasPath(const char*const path);

Q_SIGNALS:
    void    jsonChanged(void);
    void    atlasChanged(void);
};




class TkSkeleton: public elysian::SkeletonBase {
            Q_OBJECT
public:
                    TkSkeleton(TkEntity& parent);
                    TkSkeleton(void) = default;

    void            setAttrib(SkeletonAttrib * const attrib);

    virtual bool    validate(QString& msg) const override;
    virtual QMenu*  createContextMenu(void) override;

    void            setAnimByName(const int trackIndex, const char*const animName, const bool loop=true) const;

private Q_SLOTS:

    void            onJsonChanged(void);
    void            onAtlasChanged(void);

};






//============= INLINEZ=========

inline TkSkeleton::TkSkeleton(TkEntity &parent): elysian::SkeletonBase(parent) {}

inline void TkSkeletonAttrib::setAtlasPath(const char*const path) {
    SkeletonAttribBase::setAtlasPath(path);
    Q_EMIT atlasChanged();
}

inline void TkSkeletonAttrib::setJsonPath(const char*const path) {
    SkeletonAttribBase::setJsonPath(path);
    Q_EMIT jsonChanged();
}


inline void TkSkeleton::setAttrib(SkeletonAttrib * const attrib) {
    disconnect(_attrib);

    connect(attrib, SIGNAL(atlasChanged()), this, SLOT(onAtlasChanged()));
    connect(attrib, SIGNAL(jsonChanged()), this, SLOT(onJsonChanged()));

    elysian::SkeletonBase::setAttrib(attrib);
}



#endif // TKSKELETON_H

