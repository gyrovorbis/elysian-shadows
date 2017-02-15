#ifndef ELYSIAN_ENTITY_WIDGET_HPP
#define ELYSIAN_ENTITY_WIDGET_HPP

#include <gui/elysian_widget.hpp>
#include <utility/elysian_asset_typedefs.hpp>

ELYSIAN_ASSET_TYPEDEF(Entity);

namespace elysian {

    class EntityWidget: public Widget {
    private:
        Entity*         _entity = nullptr;

    public:
                        EntityWidget(void) = default;
                        EntityWidget(Entity* entity);

        Entity*         getEntity(void) const;
        void            setEntity(Entity* entity);

        //Overridden from widget
        virtual int     getPosition(AXIS axis) const override;
        virtual void    setPosition(AXIS axis, int pos) override;

        virtual int     getSize(AXIS axis) const override;
        virtual void    setSize(AXIS axis, int size) override;

        virtual void    render(void) const override;
    };




    //===== INLINEZ ======
    inline EntityWidget::EntityWidget(Entity* entity): _entity(entity) {}
    inline Entity* EntityWidget::getEntity(void) const { return _entity; }
    inline void EntityWidget::setEntity(Entity* entity) { _entity = entity; }

}

#endif // ELYSIAN_ENTITY_WIDGET_HPP
