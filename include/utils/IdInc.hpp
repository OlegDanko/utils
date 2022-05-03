#pragma once

#include <set>

template<typename T>
class IdInc {
    std::set<T> reuse;
    T highest = 0;
public:
    T get_next() {
        if(reuse.size() == 0)
            return ++highest;

        auto it = reuse.begin();
        auto id = *it;
        reuse.erase(it);
        return id;
    }
    void restore(T id) {
        if(id < highest)
            reuse.insert(id);
    }
};
