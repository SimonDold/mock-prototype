#ifndef COMPONENT_INTERNALS_H
#define COMPONENT_INTERNALS_H

#include "hash.h"

#include "utils/language.h"

#include <concepts>
#include <memory>
#include <vector>

class AbstractTask;
class BoundComponent;
class ComponentBase;

using CacheKey = std::pair<const ComponentBase *, const AbstractTask *>;
using Cache = utils::HashMap<CacheKey, std::shared_ptr<BoundComponent>>;

template<typename Tuple>
struct BoundArgs {
    using type = decltype(bind_components_recursively(
        std::declval<Tuple>(), std::declval<std::shared_ptr<AbstractTask>>(),
        std::declval<Cache &>()));
};

template<typename Args, typename T>
concept ComponentArgsFor = IsConstructibleFromArgsTuple<
    T, typename PrependedTuple<
           std::shared_ptr<AbstractTask>,
           typename BoundArgs<Args>::type>::type>::value;

template<typename Category, typename T>
concept ComponentCategoryOf = std::derived_from<Category, BoundComponent> &&
                              std::derived_from<T, Category>;

template<typename T, typename Args>
std::shared_ptr<T> make_shared_from_tuple(
    const std::shared_ptr<AbstractTask> &task, const Args &args) {
    return std::apply(
        [&](auto &&...unpackedArgs) {
            return make_shared<T>(
                task, std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
        },
        args);
}

template<typename T>
concept Bindable =
    requires(T t, const std::shared_ptr<AbstractTask> &task, Cache &cache) {
        {
            t.bind_with_cache(task, cache)
        } -> std::convertible_to<std::shared_ptr<BoundComponent>>;
    };

template<Bindable T>
auto bind_components_recursively(
    const std::shared_ptr<T> &component,
    const std::shared_ptr<AbstractTask> &task, Cache &cache) {
    return component->bind_with_cache(task, cache);
}

template<typename T>
auto bind_components_recursively(
    const std::vector<T> &vec, const std::shared_ptr<AbstractTask> &task,
    Cache &cache) {
    using BoundElementType = decltype(bind_components_recursively(
        std::declval<T>(),
        std::declval<const std::shared_ptr<AbstractTask> &>(),
        std::declval<Cache &>()));
    std::vector<BoundElementType> result;
    result.reserve(vec.size());
    for (const auto &elem : vec) {
        result.push_back(bind_components_recursively(elem, task, cache));
    }
    return result;
}

template<typename... Args>
auto bind_components_recursively(
    const std::tuple<Args...> &args, const std::shared_ptr<AbstractTask> &task,
    Cache &cache) {
    return std::apply(
        [&](const Args &...elems) {
            return std::make_tuple(
                bind_components_recursively(elems, task, cache)...);
        },
        args);
}

template<typename T>
auto bind_components_recursively(
    const T &t, const std::shared_ptr<AbstractTask> &, Cache &) {
    return t;
}

#endif
