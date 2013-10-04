
/* @brief Small, generic utility functions */
#include <vector>
#include <algorithm> /* for std::remove */
namespace elysian {

    /* @brief Iterates through haystack, erase()s all items from haystack
    whose its memory address is the same as needle's.
    @returns true if item is found and erased, false otherwise */
    template <class T, class U>
    bool containerEraseOnMatch(const T needle, U& haystack) {
        /* A modified erase/remove idiom */
        auto old_last = std::end(haystack);
        auto new_last = std::remove(std::begin(haystack),
                                    old_last,
                                    needle);
        if (old_last == new_last) {
            return false;
        }
        haystack.erase(new_last, old_last);
        return true;
    }
}
