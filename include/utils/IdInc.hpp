#pragma once

#include <set>
#include "utils.hpp"

namespace utl_prf {

template<typename T>
class IdInc {
    std::set<T> reuse;
    std::set<T> reserved;
    T highest = 0;
public:
    T get_next() {
        if(reuse.size() == 0) {
            while (is_present(++highest, reserved)) {}
            return highest;
        }

        auto it = reuse.begin();
        auto id = *it;
        reuse.erase(it);
        return id;
    }
    void restore(T id) {
        if(id < highest)
            reuse.insert(id);
    }
    bool reserve(T id) {
        if(id > highest) {
            reserved.insert(id);
            return true;
        }

        IF_PRESENT(id, reuse, it) {
            reuse.clear(it);
            return true;
        }
        return false;
    }
};

}
