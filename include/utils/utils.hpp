#ifndef UTILS_HPP
#define UTILS_HPP

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
};


}

#endif // UTILS_HPP
