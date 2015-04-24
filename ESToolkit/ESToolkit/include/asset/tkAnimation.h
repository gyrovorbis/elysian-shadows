#ifndef TK_ANIMATION_H
#define TK_ANIMATION_H

#include <QObject>
#include "elysian_animation_base.hpp"

ELYSIAN_ASSET_TYPEDEF(Entity)

class TkAnimation: public elysian::AnimationBase {
Q_OBJECT
public:

                    TkAnimation(TkEntity& parent);
                    TkAnimation(void) = default;
                    virtual ~TkAnimation(void) = default;

    //Compile-time overrides
    void            addChildEntity(Entity*const entity, const char*animName);
    void            removeChildEntity(const char*animName);
    void            removeChildEntity(Entity* entity);

    //TkComponent virtual
    virtual bool    validate(QString& str) const override;


Q_SIGNALS:
    void            entityParented(Entity*const child, Entity*const parent);
};



#endif // ANIMATION_H
