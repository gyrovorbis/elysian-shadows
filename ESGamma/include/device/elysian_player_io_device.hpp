#ifndef ELYSIAN_PLAYER_IO_DEVICE_HPP
#define ELYSIAN_PLAYER_IO_DEVICE_HPP

namespace elysian {

    class PlayerIODevice    {
    protected:
        bool        _attached;
        unsigned    _playerIndex;

    public:
        bool        isAttached(void) const;
        unsigned    getPlayerIndex(void) const;
        void        setPlayerIndex(unsigned index);

    };




    //========== INLINEZ ==========
    inline bool PlayerIODevice::isAttached(void) const { return _attached; }
    inline void PlayerIODevice::setPlayerIndex(unsigned index) { _playerIndex = index; }
    inline unsigned PlayerIODevice::getPlayerIndex(void) const { return _playerIndex; }
}

#endif // ELYSIAN_PLAYER_IO_DEVICE_HPP
