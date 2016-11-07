#ifndef TK_SKELETON_H
#define TK_SKELETON_H

#include <component/elysian_skeleton_base.hpp>

class TkAnimationTrack: public elysian::AnimationTrackBase {
public:

    static void initializeDelegates(void);
};

class TkSkeletonAttrib: public elysian::SkeletonAttribBase {
    Q_OBJECT
public:
    void        setJsonPath(const char*const path);
    void        setAtlasPath(const char*const path);

    static void initializeDelegates(void);

Q_SIGNALS:
    void        jsonChanged(void);
    void        atlasChanged(void);
};



class TkSkeleton: public elysian::SkeletonBase {
            Q_OBJECT
public:
                    TkSkeleton(TkEntity& parent, const TkSkeleton*const prefab=nullptr);
                    TkSkeleton(void) = default;

    void            setAttrib(SkeletonAttrib * const attrib);

    virtual bool    validate(QString& msg) const override;
    virtual QMenu*  createContextMenu(void) override;

    void            setAnimByName(const int trackIndex, const char*const animName, const bool loop=true);

    static void     initializeDelegates(void);

private Q_SLOTS:

    void            onJsonChanged(void);
    void            onAtlasChanged(void);

};






//============= INLINEZ=========

inline TkSkeleton::TkSkeleton(TkEntity &parent, const TkSkeleton*const prefab): elysian::SkeletonBase(parent, prefab) {}

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

