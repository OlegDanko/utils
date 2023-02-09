#pragma once

#include <memory>
#include <functional>
#include <optional>

#define IF_PRESENT(VAL, CNT, IT) if(auto IT = CNT.find(VAL); IT != CNT.end())
#define with(x) if(x; true)
#define repeat(x) for(auto repeat_i = 0; repeat_i < x; repeat_i++)


template<typename, typename>
struct tpl_type_cat;

template<typename ...Args_1, typename ...Args_2>
struct tpl_type_cat<std::tuple<Args_1...>, std::tuple<Args_2...>> {
    using tpl = std::tuple<Args_1..., Args_2...>;
};

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

template<typename T>
void apply_opt(T& target, const std::optional<T>& source) {
    target = source.value_or(target);
}


template<typename IT, typename FN>
void for_neighbors(IT it_this, IT it_end, FN fn) {
    auto it_next = std::next(it_this);
    while(it_next != it_end) {

        fn(it_this, it_next);

        it_this = it_next;
        it_next = std::next(it_this);
    }
}

template<typename IT_A, typename IT_B, typename FN>
void for_corresponding(IT_A begin_a, IT_A end_a, IT_B begin_b, IT_B end_b, FN fn) {
    while(begin_a!=end_a && begin_b!=end_b) {
        fn(begin_a, begin_b);
        ++begin_a;
        ++begin_b;
    }
}

template<typename CTN_A, typename CTN_B, typename FN>
void for_corresponding(CTN_A a, CTN_B b, FN fn) {
    for_corresponding(a.begin(), a.end(), b.begin(), b.end(), fn);
}

inline float rand_f() {
    return ((float) rand()) / (float) RAND_MAX * 2.0f - 1.0f;
}

inline float rand_f0_1() {
    return ((float) rand()) / (float) RAND_MAX;
}

template<typename IT>
class iterable {
    IT begin_;
    IT end_;
public:
    iterable(IT b, IT e) : begin_(b), end_(e) {}
    IT begin() {return begin_;}
    IT end() {return end_;}
};

}
