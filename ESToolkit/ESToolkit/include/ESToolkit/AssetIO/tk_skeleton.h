#ifndef TK_SKELETON_H
#define TK_SKELETON_H

#include <AssetIO/component/elysian_skeleton_base.hpp>

class TkAnimationTrack: public elysian::AnimationTrackBase {
public:

    static void initializeDelegates(void);
};

class TkAnimation: public elysian::AnimationBase {
public:
    static void initializeDelegates(void);
    static void initializeVirtualTableDelegates(QString property);
};

class TkSkeletonAttrib: public elysian::SkeletonAttribBase {
    Q_OBJECT
public:
    void        setJsonPath(const char*const path);
    void        setAtlasPath(const char*const path);
    void        reloadAtlas(void);
    void        reloadJson(void);

    static void initializeDelegates(void);
    static void initializeVirtualTableDelegates(QString key);

Q_SIGNALS:
    void        jsonChanged(void);
    void        atlasChanged(void);
};


class TkSkeleton: public elysian::SkeletonBase {
            Q_OBJECT
public:
                    TkSkeleton(TkEntity& parent, const TkSkeleton*const prefab=nullptr);
                    TkSkeleton(void) = default;
                    ~TkSkeleton(void);

    void            setAttrib(std::shared_ptr<SkeletonAttrib> attrib);

    void            printSlotData(void) const;

    virtual bool    validate(QString& msg) const override;
    virtual QMenu*  createContextMenu(void) override;

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

inline void TkSkeletonAttrib::reloadAtlas(void) {
    _loadAtlas();
    _loadJson();
    Q_EMIT atlasChanged();
}

inline void TkSkeletonAttrib::reloadJson(void) {
    _loadJson();
    Q_EMIT jsonChanged();
}




#endif // TKSKELETON_H

