/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_DIALOG_SCREEN_BASE_HPP
#define ELYSIAN_DIALOG_SCREEN_BASE_HPP

#include <libGyro/gyro_matrix_api.h>
#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>
#include <AssetIO/utility/elysian_asset_typedefs.hpp>

#define ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE			500

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(DialogScreen)

namespace elysian {

    class DialogScreenBase: public LuaTableProxy<DialogScreenBase, DialogScreen> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(text,           getText,    setText)
            ELYSIAN_LUA_PROXY_FIELD(trigger,        isTrigger,  setTrigger)
            //ELYSIAN_LUA_PROXY_FIELD(source,         getEntity,  addSourceEntity)
        ELYSIAN_LUA_PROXY_FIELDS_END()
    public:
        enum BUBBLE_TYPE { BUBBLE_SPEECH, BUBBLE_THOUGHT };
        enum SCREEN_TYPE { SCREEN_TEXT, SCREEN_PROMPT };
    private:
        char                _text[ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE]   = { 0 };
        GYVector2           _pos                                            = { 0.0f, 0.0f };
        Entity*             _srcEntity                                      = nullptr; //array later
        SCREEN_TYPE         _screenType;
        int                 _index                                          = -1;
        bool                _trigger                                        = false;
        //BUBBLE_TYPE		_bubbleType	= BUBBLE_SPEECH;

    protected:
                            DialogScreenBase(const SCREEN_TYPE scrType);
                            DialogScreenBase(const SCREEN_TYPE scrType, const char*const text);
                            DialogScreenBase(const DialogScreenBase& rhs);
        virtual             ~DialogScreenBase(void)=default;

        DialogScreenBase&   operator=(const DialogScreenBase& rhs);

    public:

        void                setText(const char*const text);
        const char*         getText(void) const;
        void                addSourceEntity(Entity*const entity);        //supports multiple
        void                addSourcePos(const GYVector2& pos);          //for non-entity
        unsigned            getIndex(void) const;					            //location in overall dialog
        void                setIndex(const unsigned index);
        void                setTrigger(const bool val);					//Behavior::onDialog(unsigned screen, int option)
        SCREEN_TYPE         getScreenType(void) const;
        Entity*             getEntity(void) const;
        bool                isTrigger(void) const;

    };



    //==== DIALOGSCREEN INLINEZ ====
    inline DialogScreenBase::DialogScreenBase(const SCREEN_TYPE scrType): _screenType(scrType) {}
    inline DialogScreenBase::DialogScreenBase(const SCREEN_TYPE scrType, const char*const text):
        DialogScreenBase(scrType) { strcpy(_text, text); }
    inline DialogScreenBase::DialogScreenBase(const DialogScreenBase &rhs) {
        *this = rhs;
    }
    inline void DialogScreenBase::addSourceEntity(Entity*const entity) { _srcEntity = entity; }
    inline void DialogScreenBase::addSourcePos(const GYVector2& pos) { _pos = pos; }
    inline unsigned DialogScreenBase::getIndex(void) const { return _index; }
    inline void DialogScreenBase::setIndex(const unsigned index) { _index = index; }
    inline void DialogScreenBase::setTrigger(const bool val) { _trigger = val; }
    inline auto DialogScreenBase::getScreenType(void) const -> SCREEN_TYPE { return _screenType; }
    inline Entity* DialogScreenBase::getEntity(void) const { return _srcEntity; }
    inline const char* DialogScreenBase::getText(void) const { return _text; }
    inline void DialogScreenBase::setText(const char * const text) { strcpy(_text, text); }
    inline bool DialogScreenBase::isTrigger(void) const { return _trigger; }
    inline DialogScreenBase& DialogScreenBase::operator =(const DialogScreenBase& rhs) {
        strcpy(_text, rhs._text);
        _pos        = rhs._pos;
        _srcEntity  = rhs._srcEntity;
        _screenType = rhs._screenType;
        _index      = rhs._index;
        _trigger    = rhs._trigger;
        return *this;
    }

}

#endif // ELYSIAN_DIALOG_SCREEN_BASE_HPP

