#ifndef TEST_MEMORY_MAPPED_BYTE_ARRAY_H
#define TEST_MEMORY_MAPPED_BYTE_ARRAY_H

#include <QObject>
#include <QDebug>
#include <QtTest>

template<typename Derived>
class MemoryMappedByteArray {
public:

    size_t 		getSize(void) const;
    bool 		setByte(size_t offset, uint8_t value);
    uint8_t 	getByte(size_t offset) const;

    template<typename T>
    bool 		extract(size_t offset, T* data);

    bool 		memcpy(void* data, size_t offset, size_t size);
    uint8_t     operator[](size_t offset);
};

template<typename Derived>
inline size_t MemoryMappedByteArray<Derived>::getSize(void) const {
    return static_cast<const Derived*>(this)->_getSize();
}

template<typename Derived>
inline bool MemoryMappedByteArray<Derived>::setByte(size_t offset, uint8_t value) {
    return static_cast<Derived*>(this)->_setByte(offset, value);
}

template<typename Derived>
inline uint8_t MemoryMappedByteArray<Derived>::getByte(size_t offset) const {
    return static_cast<const Derived*>(this)->_getByte(offset);
}


template<typename Derived>
template<typename T>
inline bool MemoryMappedByteArray<Derived>::extract(size_t offset, T* data) {
    return memcpy(data, offset, sizeof(T));
}

template<typename Derived>
inline bool MemoryMappedByteArray<Derived>::memcpy(void* data, size_t offset, size_t size) {
    if(offset + size > getSize()) return false;

    for(size_t i = 0; i < size; ++i) {
        *(static_cast<uint8_t*>(data)+i) = getByte(offset + i);
    }

    return true;
}

template<typename Derived>
inline uint8_t MemoryMappedByteArray<Derived>::operator[](size_t offset) {
    return getByte(offset);
}


class DynamicMemoryMappedByteArray:
    public MemoryMappedByteArray<DynamicMemoryMappedByteArray>
{

    friend class MemoryMappedByteArray<DynamicMemoryMappedByteArray>;

public:
    struct MemoryRegion {
                        MemoryRegion(void* _buffer, size_t _size);

        void*           buffer 		= nullptr;
        size_t          size 		= 0;
    };

    std::vector<MemoryRegion>
                    _regions;

protected:

    size_t          _getSize(void) const;
    uint8_t         _getByte(size_t offset) const;
    bool            _setByte(size_t offset, uint8_t value);
    const uint8_t*  _offsetToAddr(size_t offset) const;
    uint8_t*        _offsetToAddr(size_t offset);

    template<typename T>
    inline void     _appendRegions(T ptr);
    template<typename T, typename... Args>
    void            _appendRegions(T ptr, Args... other);


public:
                    DynamicMemoryMappedByteArray(void) = default;
                    DynamicMemoryMappedByteArray(const DynamicMemoryMappedByteArray& lRef) = default;
                    DynamicMemoryMappedByteArray(DynamicMemoryMappedByteArray&& rRef) = default;
                    DynamicMemoryMappedByteArray(std::initializer_list<MemoryRegion> regions);
    template<typename... Args>
                    DynamicMemoryMappedByteArray(Args... args);


    size_t          getRegionCount(void) const;
    void            setMemoryRegion(size_t index, void* buffer, size_t size);
    auto            getMemoryRegion(size_t index) const -> const MemoryRegion*;

    template<typename T>
    void            appendMemoryRegion(T* t);
    void            appendMemoryRegion(void* buffer, size_t size);
    void            insertMemoryRegion(size_t index, void* buffer, size_t size);
    void            eraseMemoryRegion(size_t index);
    void            removeMemoryRegion(void* buffer);
};



namespace {

    template<typename T>
    struct DynamicMemoryMappedByteArrayUtil {
        static std::enable_if_t<std::is_pointer<T>::value, DynamicMemoryMappedByteArray::MemoryRegion> createRegion(T address) {
            return DynamicMemoryMappedByteArray::MemoryRegion(address, sizeof(typename std::remove_pointer<T>::type));
        }
    };

    template<>
    struct DynamicMemoryMappedByteArrayUtil<DynamicMemoryMappedByteArray::MemoryRegion> {
        static DynamicMemoryMappedByteArray::MemoryRegion createRegion(DynamicMemoryMappedByteArray::MemoryRegion region) {
            return region;
        }
    };

}

inline size_t DynamicMemoryMappedByteArray::_getSize(void) const {
    size_t size = 0;
    for(size_t e = 0; e < _regions.size(); ++e) {
        size += _regions[e].size;
    }
    return size;
}

inline const uint8_t* DynamicMemoryMappedByteArray::_offsetToAddr(size_t offset) const {
    size_t curOffset = 0;

    for(size_t e = 0; e < _regions.size(); ++e) {

        if(offset < curOffset + _regions[e].size) {
            if(_regions[e].buffer) {
                return static_cast<uint8_t*>(_regions[e].buffer) + (offset - curOffset);
            } else {
                return nullptr;
            }
        }

        curOffset += _regions[e].size;
    }

    return nullptr;
}


inline uint8_t DynamicMemoryMappedByteArray::_getByte(size_t offset) const {
    const uint8_t* addr = _offsetToAddr(offset);
    return addr? *addr : 0;
}

inline bool DynamicMemoryMappedByteArray::_setByte(size_t offset, uint8_t value) {
    uint8_t* addr = _offsetToAddr(offset);
    if(addr) {
        *addr = value;
        return true;
    } else {
        return false;
    }
}

inline uint8_t* DynamicMemoryMappedByteArray::_offsetToAddr(size_t offset)  {
    return const_cast<uint8_t*>(static_cast<const DynamicMemoryMappedByteArray*>(this)->_offsetToAddr(offset));
}


template<typename T>
inline void DynamicMemoryMappedByteArray::_appendRegions(T ptr) {
    _regions.push_back(DynamicMemoryMappedByteArrayUtil<T>::createRegion(ptr));
}

template<typename T, typename... Args>
inline void DynamicMemoryMappedByteArray::_appendRegions(T ptr, Args... other) {
    _appendRegions(ptr);
    _appendRegions<Args...>(other...);
}

template<typename T>
inline void DynamicMemoryMappedByteArray::appendMemoryRegion(T* t) {
    appendMemoryRegion(t, sizeof(T));
}

inline void DynamicMemoryMappedByteArray::appendMemoryRegion(void* buffer, size_t size) {
    _regions.emplace_back(buffer, size);
}

inline size_t DynamicMemoryMappedByteArray::getRegionCount(void) const { return _regions.size(); }

inline void DynamicMemoryMappedByteArray::insertMemoryRegion(size_t index, void* buffer, size_t size) {
    _regions.emplace(_regions.begin() + static_cast<unsigned>(index), buffer, size );
}

inline void DynamicMemoryMappedByteArray::eraseMemoryRegion(size_t index) {
    _regions.erase(_regions.begin() + static_cast<unsigned>(index));
}

inline void DynamicMemoryMappedByteArray::removeMemoryRegion(void* buffer) {
    for(size_t i = 0; i < getRegionCount(); ++i) {
        if(_regions[i].buffer == buffer) {
            eraseMemoryRegion(i);
            return;
        }
    }
}

inline void DynamicMemoryMappedByteArray::setMemoryRegion(size_t index, void* buffer, size_t size) {
    assert(index < getRegionCount());
    _regions[index].buffer  = buffer;
    _regions[index].size    = size;
}

inline auto DynamicMemoryMappedByteArray::getMemoryRegion(size_t index) const -> const MemoryRegion* {
    return index < getRegionCount()? &_regions[index] : nullptr;
}

template<typename... Args>
inline DynamicMemoryMappedByteArray::DynamicMemoryMappedByteArray(Args... args) {
    _appendRegions(args...);
}


inline DynamicMemoryMappedByteArray::DynamicMemoryMappedByteArray(std::initializer_list<MemoryRegion> regions):
    _regions(std::move(regions))
{}

inline DynamicMemoryMappedByteArray::MemoryRegion::MemoryRegion(void* _buffer, size_t _size):
    buffer(_buffer),
    size(_size)
{}



struct MemoryMap: public DynamicMemoryMappedByteArray {
    char buffer[50] = "abcdefghijklmnopqrstuvwxyz";

    MemoryMap(void):
        DynamicMemoryMappedByteArray({
            { buffer, 4 },          //"abcd"
            { &buffer[4], 3 },      //"efg"
            { &buffer[7], 6 },      //"hijklm"
            { &buffer[20], 2 },     //"uv"
            { &buffer[15], 4 }      //"pqrs"
        })
    {}
};


namespace tktest {

class MemoryMappedByteArrayTest: public QObject{
    Q_OBJECT

public:
        MemoryMappedByteArrayTest(void) = default;
private Q_SLOTS:


    void constructDefault(void);
    void constructInitializerList(void);
    void constructVariadic(void);
    void regionGet(void);
    void regionSet(void);
    void readBytes(void);
    void writeBytes(void);

    //Finish me
    //void append(void);
    //void insert(void);
    //void erase(void);
    //void remove(void);

};


inline void MemoryMappedByteArrayTest::constructDefault(void) {
    DynamicMemoryMappedByteArray ba;

    QVERIFY(!ba.getSize());
    QVERIFY(!ba.getRegionCount());
}


inline void MemoryMappedByteArrayTest::constructInitializerList(void) {
    char buffer[] = "abcdefghijklmnopqrstuvwxyz";

    auto a =
        DynamicMemoryMappedByteArray({
            { buffer, 4 },          //"abcd"
            { &buffer[4], 3 },      //"efg"
            { &buffer[7], 6 },      //"hijklm"
            { &buffer[20], 2 },     //"uv"
            { &buffer[15], 4 }      //"pqrs"
        });

    QVERIFY(a.getRegionCount() == 5);
    QVERIFY(a.getSize() == 19);

}

inline void MemoryMappedByteArrayTest::constructVariadic(void) {
    float a;
    int b;
    char str[1000];

    struct Blah {
        char c;
        bool d;
    } blah;

    DynamicMemoryMappedByteArray bb = {
        &a,
        &b,
        DynamicMemoryMappedByteArray::MemoryRegion{ str, sizeof(str) },
        &blah
    };

    QVERIFY(bb.getSize() == sizeof(float) + sizeof(int) + sizeof(str) + sizeof(Blah));
    QVERIFY(bb.getRegionCount() == 4);
}


inline void MemoryMappedByteArrayTest::regionGet(void) {
    char buffer[50] = "abcdefghijklmnopqrstuvwxyz";

    auto a =
        DynamicMemoryMappedByteArray({
            { buffer, 4 },          //"abcd"
            { &buffer[4], 3 },      //"efg"
            { &buffer[7], 6 },      //"hijklm"
            { &buffer[20], 2 },     //"uv"
            { &buffer[15], 4 }      //"pqrs"
        });

    auto* region = a.getMemoryRegion(3);

    QVERIFY(region->buffer == &buffer[20]);
    QVERIFY(region->size == 2);

}

inline void MemoryMappedByteArrayTest::regionSet(void) {
    char buffer[50] = "abcdefghijklmnopqrstuvwxyz";

    auto a =
        DynamicMemoryMappedByteArray({
            { buffer, 4 },          //"abcd"
            { &buffer[4], 3 },      //"efg"
            { &buffer[7], 6 },      //"hijklm"
            { &buffer[20], 2 },     //"uv"
            { &buffer[15], 4 }      //"pqrs"
        });

    a.setMemoryRegion(2, &buffer[2], 3);
    auto* region = a.getMemoryRegion(2);

    QVERIFY(region->buffer == &buffer[2]);
    QVERIFY(region->size == 3);

}

inline void MemoryMappedByteArrayTest::readBytes(void) {
    MemoryMap map;

    char expected[] = "abcdefghijklmuvpqrs";

    for(size_t i = 0; i < sizeof(expected); ++i) {
        QVERIFY(map.getByte(i) == expected[i]);
        QVERIFY(map[i] == expected[i]);
    }
}


inline void MemoryMappedByteArrayTest::writeBytes(void) {
    MemoryMap map;

    for(size_t i = 0; i < map.getSize(); ++i) {
        map.setByte(i, static_cast<uint8_t>(i));
    }

    char expected[] = "abcdefghijklmuvpqrs";

    size_t curOffset = 0;
    auto insertChars = [&](size_t offset, size_t count) {
        for(size_t i = offset; i < offset + count; ++i) {
            expected[curOffset] = static_cast<char>(curOffset);
            ++curOffset;
        }

    };

    insertChars(0, 4);
    insertChars(4, 3);
    insertChars(7, 6);
    insertChars(20, 2);
    insertChars(15, 4);

    for(size_t i = 0; i < sizeof(expected); ++i) {
        qDebug() << i << map[i] << expected[i];
        QVERIFY(map[i] == expected[i]);
    }

}


}

#endif // TEST_MEMORY_MAPPED_BYTE_ARRAY_H
