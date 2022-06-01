#pragma once

#include <memory>
#include <functional>

#define IF_PRESENT(VAL, CNT, IT) if(auto IT = CNT.find(VAL); IT != CNT.end())
#define with(x) if(x; true)

namespace utl_prf {

template<typename T>
class on_del_exec {
    std::unique_ptr<T> t;
    using fn_t = std::function<void(std::unique_ptr<T>)>;
    fn_t fn;
public:
    on_del_exec(std::unique_ptr<T> t, fn_t fn) : t(std::move(t)), fn(fn) {}
    ~on_del_exec() { if(fn) fn(std::move(t)); }
    T* operator->() { return t.get();}
    operator bool() { return t != nullptr;}
};

template<typename V, typename CNT>
bool is_present(const V& val, CNT& container) {
    return container.find(val) != container.end();
}


template<typename V, typename CNT, typename CALLBACK>
void if_present(const V& val, CNT& container, CALLBACK&& callback) {
    IF_PRESENT(val, container, it)
        callback(it);
}

template<typename>
struct pair_maker;

template<typename V1, typename V2>
struct pair_maker<std::pair<V1, V2>> {
    static std::pair<V1, V2> make(V1&& v1, V2&& v2) { return std::make_pair<V1, V2>(std::move(v1), std::move(v2)); }
};


}
