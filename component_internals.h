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

using CacheKey = const std::pair<const ComponentBase *, const AbstractTask *>;
using Cache = utils::HashMap<CacheKey, std::shared_ptr<BoundComponent>>;

template<typename... Args>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task, Cache &cache,
    const std::tuple<Args...> &args);

template<typename Tuple>
struct BoundArgs {
    using type = decltype(recursively_bind_components(
        std::declval<std::shared_ptr<AbstractTask>>(), std::declval<Cache &>(),
        std::declval<Tuple>()));
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
static std::shared_ptr<T> make_shared_from_tuple(
    const std::shared_ptr<AbstractTask> &task, const Args &args) {
    return std::apply(
        [&](auto &&...unpackedArgs) {
            return make_shared<T>(
                task, std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
        },
        args);
}

template<typename T>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &, Cache &, const T &t) {
    return t;
}

template<typename T>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task, Cache &cache,
    const std::vector<T> &vec) {
    using BoundElementType = decltype(recursively_bind_components(
        std::declval<const std::shared_ptr<AbstractTask> &>(),
        std::declval<Cache &>(), std::declval<T>()));
    std::vector<BoundElementType> result;
    result.reserve(vec.size());
    for (const auto &elem : vec) {
        result.push_back(recursively_bind_components(task, cache, elem));
    }
    return result;
}

template<typename... Args>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task, Cache &cache,
    const std::tuple<Args...> &args) {
    return std::apply(
        [&](const Args &...elems) {
            return std::make_tuple(
                recursively_bind_components(task, cache, elems)...);
        },
        args);
}

template<typename T>
concept Bindable =
    requires(T t, const std::shared_ptr<AbstractTask> &task, Cache &cache) {
        { t.bind_with_cache(task, cache) };
    };

template<Bindable T>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task, Cache &cache,
    const std::shared_ptr<T> &component) {
    return component->bind_with_cache(task, cache);
}

#endif
