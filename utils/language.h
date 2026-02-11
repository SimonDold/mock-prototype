#ifndef UTILS_LANGUAGE_H
#define UTILS_LANGUAGE_H

#include <concepts>
#include <tuple>

namespace utils {

template<typename T, typename Tuple>
struct IsConstructibleFromArgsTuple;

template<typename T, typename... Ts>
struct IsConstructibleFromArgsTuple<T, std::tuple<Ts...>> {
    static constexpr bool value = std::constructible_from<T, Ts...>;
};

template<typename T, typename Args>
concept ConstructibleFromArgsTuple = IsConstructibleFromArgsTuple<
    T, Args>::value;
}
#endif

