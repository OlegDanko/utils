#pragma once

#include <cstddef>
#include <type_traits>
#include <tuple>

namespace utl_prf {

template <typename... Ts>
struct types {
    using tpl = std::tuple<Ts...>;
    static constexpr size_t count = std::tuple_size<tpl>();
};

template<typename, typename>
struct cat_types;
template<typename T, typename ...Ts>
struct cat_types<T, types<Ts...>> {
    using types_ = types<T, Ts...>;
};
template<typename T, typename ...Ts>
struct cat_types<types<Ts...>, T> {
    using types_ = types<Ts..., T>;
};
template<typename ...Ts1, typename ...Ts2>
struct cat_types<types<Ts1...>, types<Ts2...>> {
    using types_ = types<Ts1..., Ts2...>;
};

template<template<typename> typename, typename...>
struct type_apply;
template<template<typename> typename T, typename T1, typename...Ts>
struct type_apply<T, T1, Ts...> {
    using types_ = typename cat_types<T<T1>, typename type_apply<T, Ts...>::types_>::types_;
};
template<template<typename> typename T>
struct type_apply<T> {
    using types_ = types<>;
};
template<template<typename> typename T, typename ...Ts>
struct type_apply<T, types<Ts...>> {
    using types_ = typename type_apply<T, Ts...>::types_;
};

template<typename, typename...>
struct is_type_unique;
template<typename T>
struct is_type_unique<T> { static constexpr bool val = true; };
template<typename T, typename T1, typename ...Ts>
struct is_type_unique<T, T1, Ts...> { static constexpr bool val = (!std::is_same<T, T1>::value) && is_type_unique<T, Ts...>::val; };
template<typename T, typename... Ts>
static constexpr bool is_type_unique_v = is_type_unique<T, Ts...>::val;

template<typename, typename...>
struct is_type_present;
template<typename T>
struct is_type_present<T> { static constexpr bool val = false; };
template<typename T, typename T1, typename ...Ts>
struct is_type_present<T, T1, Ts...> { static constexpr bool val = (std::is_same<T, T1>::value) || is_type_present<T, Ts...>::val; };

template<typename T, typename ...Ts>
struct is_type_present<T, types<Ts...>> { static constexpr bool val = is_type_present<T, Ts...>::val; };


template<typename T, typename... Ts>
static constexpr bool is_type_present_v = is_type_present<T, Ts...>::val;


template<typename ... Ts>
struct unique_types {
    template<typename C, typename ...As>
    struct collector;
    template<typename CollectedPrev, typename A>
    struct collector<CollectedPrev, A> {
        using types_ = typename std::conditional<!is_type_present_v<A, CollectedPrev>,
                                                 typename cat_types<CollectedPrev, A>::types_,
                                                 CollectedPrev>::type;
    };
    template<typename UNIQUES_PREV, typename A, typename ...As>
    struct collector<UNIQUES_PREV, A, As...> {
        using uniques = typename collector<UNIQUES_PREV, A>::types_;
        using types_ = typename collector<uniques, As...>::types_;
    };
public:
    using types_ = typename collector<types<>, Ts...>::types_;
};

template<std::size_t I>
struct size_ {
    static constexpr std::size_t val = I;
};

template<typename T, typename ...Ts>
struct index_of_type {
    template<typename A, typename ...As>
    struct index_of_type_;
    template<typename A>
    struct index_of_type_<A> {
        static constexpr size_t i = 0;
    };
    template<typename A, typename A1, typename ...As>
    struct index_of_type_<A, A1, As...> {
        static constexpr size_t i =
                std::conditional<std::is_same_v<A, A1>,
                                 size_<0>,
                                 size_<1 + index_of_type_<A, As...>::i>>::type::val;
    };

    static_assert(is_type_present_v<T, Ts...>, "In index_of_type, type is not found in the type list");
    static constexpr size_t i = index_of_type_<T, Ts...>::i;
};

template<typename T, typename ...Ts>
struct index_of_type<T, types<Ts...>> {
    static constexpr size_t i = index_of_type<T, Ts...>::i;
};

template<typename T, typename ...Ts>
constexpr size_t index_of_type_v = index_of_type<T, Ts...>::i;

}
