#ifndef ELYSIAN_ASSOCIATIVE_AGGREGATE_ARRAY_HPP
#define ELYSIAN_ASSOCIATIVE_AGGREGATE_ARRAY_HPP

#include <vector>
#include <cassert>

namespace elysian {

    template<typename F, typename S, template <typename...> class C>
    class AssociativeAggregateArray;

    //Global AssociativeAggregateArray overloaded operator forward declarations
    template<typename F, typename S, template <typename...> class C>
    AssociativeAggregateArray<F, S, C>  operator+(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2);
    template<typename F, typename S, template <typename...> class C>
    AssociativeAggregateArray<F, S, C>  operator-(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2);
    template<typename F, typename S, template <typename...> class C>
    bool                                operator==(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2);
    template<typename F, typename S, template <typename...> class C>
    bool                                operator!=(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2);

    template<typename F,
             typename S,
             template <typename...> class C = std::vector>
    class AssociativeAggregateArray {
    public:
        struct Entry {
            F               first           = nullptr;
            S               second;

                            Entry(void)     = default;

                            //Construct from a universal reference of F-type only.
                            template<typename F1,    //SFINAE-check universal reference type
                                    typename = typename std::enable_if_t<std::is_same<F, std::remove_reference_t<F1>>::value>>
                            Entry(F1&& _first);

                            //Construct form a universal reference of both an F and an S type.
                            template<typename F1,   //SFINAE-check universal reference types
                                    typename S1,
                                    typename = std::enable_if_t<std::is_same<F, std::remove_reference_t<F1>>::value>,
                                    typename = std::enable_if_t<std::is_same<S, std::remove_reference_t<S1>>::value>>
                            Entry(F1&& _first, S1&& _second);

            //overloaded operators (internal)
            F               operator->(void);
            F               operator->(void) const;
                            operator F(void) const;
                            operator const F(void) const;

            //overloaded operators (external)
            bool            operator==(const Entry& rhs) const;
            bool            operator!=(const Entry& rhs) const;
        };

                            AssociativeAggregateArray(void) = default;
                            AssociativeAggregateArray(Entry entry);
                            AssociativeAggregateArray(C<Entry>&& entries);
                            AssociativeAggregateArray(const C<Entry>& entries);
                            AssociativeAggregateArray(const AssociativeAggregateArray& other);
                            AssociativeAggregateArray(AssociativeAggregateArray&& other);

        void                addEntry(Entry entry);
        void                addEntries(const C<Entry>& itemSet);
        unsigned            addNewEntries(const C<Entry>& itemSet);
        bool                removeEntry(const F& first);
        unsigned            removeEntries(const C<F>& itemSet);
        void                clear(void);

        unsigned            getCount(void) const;

        auto                begin(void) -> typename C<Entry>::iterator;
        auto                end(void) -> typename C<Entry>::iterator;
        auto                begin(void) const -> typename C<Entry>::const_iterator;
        auto                end(void) const -> typename C<Entry>::const_iterator;
        auto                cbegin(void) const -> typename C<Entry>::const_iterator;
        auto                cend(void) const -> typename C<Entry>::const_iterator;

        Entry*              findEntry(const F& first);
        const Entry*        findEntry(const F& first) const;

        Entry*              getEntry(unsigned index);
        const Entry*        getEntry(unsigned index) const;
        const Entry*        operator[](const F& first) const;
        Entry*              operator[](const F& first);
        const Entry*        operator[](unsigned index) const;
        Entry*              operator[](unsigned index);

        //Overloaded operators (internal)
        const AssociativeAggregateArray<F, S, C>& operator+=(const AssociativeAggregateArray<F, S, C>& rhs);
        const AssociativeAggregateArray<F, S, C>& operator-=(const AssociativeAggregateArray<F, S, C>& rhs);
        const AssociativeAggregateArray<F, S, C>& operator=(AssociativeAggregateArray<F, S, C>&& rhs);
        const AssociativeAggregateArray<F, S, C>& operator=(const AssociativeAggregateArray<F, S, C>& rhs);

        //Overloaded operators (external)
        friend AssociativeAggregateArray<F, S, C>  operator+<>(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2);
        friend AssociativeAggregateArray<F, S, C>  operator-<>(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2);
        friend bool                                operator==<>(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2);
        friend bool                                operator!=<>(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2);

    private:
        C<Entry>            _entries;
    };











    //======== INLINEZ =======
    template<typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C>::AssociativeAggregateArray(Entry entry) { _entries.push_back(std::move(entry)); }
    template<typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C>::AssociativeAggregateArray(C<Entry>&& entries): _entries(entries) {}
    template<typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C>::AssociativeAggregateArray(const C<Entry>& entries): _entries(entries) {}

    template<typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C>::AssociativeAggregateArray(const AssociativeAggregateArray& other): _entries(other._entries) {}
    template<typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C>::AssociativeAggregateArray(AssociativeAggregateArray&& other): _entries(other._entries) {}

    template<typename F, typename S, template <typename...> class C>
    inline void AssociativeAggregateArray<F, S, C>::addEntry(Entry entry) {
        _entries.push_back(std::move(entry));
    }

    template<typename F, typename S, template <typename...> class C>
    inline bool AssociativeAggregateArray<F, S, C>::removeEntry(const F& first) {
        for(auto it =  _entries.cbegin(); it != _entries.cend(); ++it) {
            if((*it).first == first) {
                _entries.erase(it);
                return true;
            }
        }
        return false; //never found shit
    }

    template<typename F, typename S, template <typename...> class C>
    void AssociativeAggregateArray<F, S, C>::addEntries(const C<Entry>& itemSet) {
        for(auto& i : itemSet) addEntry(i);
    }

    template<typename F, typename S, template <typename...> class C>
    unsigned AssociativeAggregateArray<F, S, C>::addNewEntries(const C<Entry>& itemSet) {
        unsigned count = 0;
        for(auto& i : itemSet) {
            if(!findEntry(i.first)) {
                addEntry(i);
                ++count;
            }
        }
        return count;
    }

    template<typename F, typename S, template <typename...> class C>
    unsigned AssociativeAggregateArray<F, S, C>::removeEntries(const C<F>& itemSet) {
        unsigned count = 0;
        for(auto& i : itemSet) count += (int)removeEntry(i);
        return count;
    }

    template<typename F, typename S, template <typename...> class C>
    inline unsigned AssociativeAggregateArray<F, S, C>::getCount(void) const {
        return _entries.size();
    }

    template<typename F, typename S, template <typename...> class C>
    inline void AssociativeAggregateArray<F, S, C>::clear(void) {
        _entries.clear();
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::begin(void) -> typename C<Entry>::iterator {
        return _entries.begin();
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::end(void) -> typename C<Entry>::iterator {
        return _entries.end();
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::begin(void) const -> typename C<Entry>::const_iterator {
        return _entries.cbegin();
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::end(void) const -> typename C<Entry>::const_iterator {
        return _entries.cend();
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::cbegin(void) const -> typename C<Entry>::const_iterator {
        return _entries.cbegin();
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::cend(void) const -> typename C<Entry>::const_iterator {
        return _entries.cend();
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::findEntry(const F& first) -> Entry* {
        for(auto& i: _entries) {
            if(i.first == first) return &i;
        }
        return nullptr;
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::findEntry(const F& first) const -> const Entry* {
        for(auto& i: _entries) {
            if(i.first == first) return &i;
        }
        return nullptr;
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::getEntry(unsigned index) -> Entry* {
        assert(index < _entries.size());
        return &_entries[index];
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::getEntry(unsigned index) const -> const Entry* {
        assert(index < _entries.size());
        return &_entries[index];
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::operator[](const F& first) const ->  const Entry* {
        return findEntry(first);
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::operator[](const F& first) -> Entry* {
        return findEntry(first);
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::operator[](unsigned index) const -> const Entry*  {
        return getEntry(index);
    }

    template<typename F, typename S, template <typename...> class C>
    inline auto AssociativeAggregateArray<F, S, C>::operator[](unsigned index) -> Entry* {
        return getEntry(index);
    }

    template<typename F, typename S, template <typename...> class C>
    inline const AssociativeAggregateArray<F, S, C>& AssociativeAggregateArray<F, S, C>::operator+=(const AssociativeAggregateArray<F, S, C>& rhs) {
        for(const auto& i : rhs) {
            if(!findEntry(i.first)) {
                addEntry(i);
            }
        }
        return *this;
    }
    template<typename F, typename S, template <typename...> class C>
    inline const AssociativeAggregateArray<F, S, C>& AssociativeAggregateArray<F, S, C>::operator-=(const AssociativeAggregateArray<F, S, C>& rhs) {
        for(const auto& i : rhs) {
            if(findEntry(i.first)) {
                removeEntry(i.first);
            }
        }
        return *this;
    }
    template<typename F, typename S, template <typename...> class C>
    inline const AssociativeAggregateArray<F, S, C>& AssociativeAggregateArray<F, S, C>::operator=(AssociativeAggregateArray<F, S, C>&& rhs) {
        _entries = rhs._entries;
        return *this;
    }

    template<typename F, typename S, template <typename...> class C>
    inline const AssociativeAggregateArray<F, S, C>& AssociativeAggregateArray<F, S, C>::operator=(const AssociativeAggregateArray<F, S, C>& rhs) {
        _entries = rhs._entries;
        return *this;
    }

    //AssociativeAggregateArray::Entry
    template <typename F, typename S, template <typename...> class C>
    template <typename F1, typename>
    inline AssociativeAggregateArray<F, S, C>::Entry::Entry(F1&& _first):
        first(std::forward<F1>(_first))
    {}

    template <typename F, typename S, template <typename...> class C>
    template <typename F1, typename S1, typename, typename>
    inline AssociativeAggregateArray<F, S, C>::Entry::Entry(F1&& _first, S1&& _second):
        first(std::forward<F1>(_first)),
        second(std::forward<S1>(_second))
    {}

    template <typename F, typename S, template <typename...> class C>
    inline F AssociativeAggregateArray<F, S, C>::Entry::operator->(void) {
        return first;
    }

    template <typename F, typename S, template <typename...> class C>
    inline F AssociativeAggregateArray<F, S, C>::Entry::operator->(void) const {
        return first;
    }


    template <typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C>::Entry::operator F(void) const {
        return first;
    }

    template <typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C>::Entry::operator const F(void) const {
        return first;
    }

    //Global overloaded operators
    template<typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C> operator+(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2) {
        AssociativeAggregateArray<F, S, C> dest = s1;
        dest += s2;
        return dest;
    }

    template<typename F, typename S, template <typename...> class C>
    inline AssociativeAggregateArray<F, S, C> operator-(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2) {
        AssociativeAggregateArray<F, S, C> dest = s1;
        dest -= s2;
        return dest;
    }

    template<typename F, typename S, template <typename...> class C>
    inline bool operator==(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2) {
        if(s1.getCount() != s2.getCount()) return false;

        if(s1.getCount() == 0) return true;

        for(unsigned i = 0; i < s1.getCount(); ++i) {
            if(*s1.getEntry(i) != *s2.getEntry(i)) return false;
        }
        return true;
    }

    template<typename F, typename S, template <typename...> class C>
    inline bool operator!=(const AssociativeAggregateArray<F, S, C>& s1, const AssociativeAggregateArray<F, S, C>& s2) {
        return !(s1 == s2);
    }

    template<typename F, typename S, template <typename...> class C>
    inline bool AssociativeAggregateArray<F, S, C>::Entry::operator==(const typename AssociativeAggregateArray<F, S, C>::Entry& rhs) const {
        return (this->first == rhs.first && this->second == rhs.second);

    }

    template<typename F, typename S, template <typename...> class C>
    inline bool AssociativeAggregateArray<F, S, C>::Entry::operator!=(const typename AssociativeAggregateArray<F, S, C>::Entry& rhs) const {
        return !(*this == rhs);

    }

}

#endif // ELYSIAN_ASSOCIATIVE_AGGREGATE_ARRAY_HPP
