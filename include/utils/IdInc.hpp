#pragma once

#include <unordered_set>
#include <queue>
#include <limits>
#include <stdexcept>
#include "utils.hpp"

namespace utl_prf {

template<typename T>
class IdInc {
    std::queue<T> reuse_queue;
    std::unordered_set<T> reuse_set;
    std::unordered_set<T> reserved;
    T highest = 0;
public:
    T get_next() {
        while (highest < std::numeric_limits<T>::max() && is_present(++highest, reserved)) {}

        if(highest < std::numeric_limits<T>::max())
            return highest;

        if(reuse_queue.empty())
            throw std::runtime_error("IdInc has run out of IDs");

        while(is_present(reuse_queue.front(), reserved))
              reuse_queue.pop();

        auto id = reuse_queue.front();

        IF_PRESENT(id, reuse_set, it) {
            reuse_set.erase(it);
        }

        reuse_queue.pop();
        return id;
    }
    void restore(T id) {
        if(id >= highest)
            return;

        reuse_set.insert(id);
        reuse_queue.push(id);
    }
    void reserve(T id) {
        if(id > highest) {
            reserved.insert(id);
            return;
        }

        IF_PRESENT(id, reuse_set, it) {
            reuse_set.erase(it);
            return;
        }
        throw std::runtime_error("IdInc could not reserve id");
    }
};

}
