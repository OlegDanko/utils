#pragma once

#include <memory>
#include <functional>

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
    if(auto it = container.find(val); it != container.end())
        callback(it);
}

}
