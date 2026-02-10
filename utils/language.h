#ifndef UTILS_LANGUAGE_H
#define UTILS_LANGUAGE_H

#include <concepts>
#include <tuple>

template<typename T, typename Tuple>
struct PrependedTuple;

template<typename T, typename... Ts>
struct PrependedTuple<T, std::tuple<Ts...>> {
    using type = std::tuple<T, Ts...>;
};

template<typename T, typename Tuple>
struct IsConstructibleFromArgsTuple;

template<typename T, typename... Ts>
struct IsConstructibleFromArgsTuple<T, std::tuple<Ts...>> {
    static constexpr bool value =
        std::constructible_from<T, Ts...>;
};

#endif
