#pragma once

#include <mutex>

namespace utl_prf {

template<typename T>
struct Protected {
    std::mutex mtx;
    T val;
    struct Locked {
        T& val;
        std::lock_guard<std::mutex> lk;

        T& get() {
            return val;
        }
    };

    Locked lock() {
        return {val, std::lock_guard{mtx}};
    }
};

}
