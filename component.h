#ifndef COMPONENT_H
#define COMPONENT_H

#include "hash.h"
#include "utils.h"
#include "utils/language.h"

#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class AbstractTask;

// common base class for all bound components (OpneLists, Evaluators, etc)
// they are *bound* to a task.
class BoundComponent {
public:
    virtual ~BoundComponent() = default;
};

// common base class for templated classes about TypedComponent and Components
class ComponentBase {
public:
    virtual ~ComponentBase() = default;
};

using CacheKey = const std::pair<const ComponentBase *, const AbstractTask *>;
using Cache = utils::HashMap<CacheKey, std::shared_ptr<BoundComponent>>;

template<typename... Args>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task, Cache &cache,
    const std::tuple<Args...> &args);

template<typename Tuple>
struct BoundArgs {
    using type = decltype(recursively_bind_components(
        std::declval<std::shared_ptr<AbstractTask>>(),
        std::declval<Cache &>(), std::declval<Tuple>()));
};

template<typename BoundComponent, typename ComponentArgs>
struct BoundComponentMatchesComponentArgs {
    static constexpr bool value = IsConstructibleFromArgsTuple<
        BoundComponent,
        typename PrependedTuple<
            std::shared_ptr<AbstractTask>,
            typename BoundArgs<ComponentArgs>::type>::type>::value;
};

template<typename BoundComponent, typename BoundComponentArgs>
static std::shared_ptr<BoundComponent> make_shared_from_tuple(
    const std::shared_ptr<AbstractTask> &task, const BoundComponentArgs &args) {
    return std::apply(
        [&](auto &&...unpackedArgs) {
            return make_shared<BoundComponent>(
                task, std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
        },
        args);
}

template<typename BoundComponentType>
class TypedComponent : public ComponentBase {
protected:
    const std::string description;
    const utils::Verbosity verbosity;
    // // mutable utils::LogProxy log;

    virtual std::shared_ptr<BoundComponentType> create_bound_component(
        const std::shared_ptr<AbstractTask> &task,
        Cache &cache) const = 0;

public:
    TypedComponent(const std::string &description, utils::Verbosity verbosity)
        : description(description), verbosity(verbosity) {
    }

    std::shared_ptr<BoundComponentType> bind_with_cache(
        const std::shared_ptr<AbstractTask> &task, Cache &cache) const {
        std::shared_ptr<BoundComponentType> component;
        const CacheKey key = std::make_pair(this, task.get());
        if (cache.count(key)) {
            component =
                std::dynamic_pointer_cast<BoundComponentType>(cache.at(key));
        } else {
            component = create_bound_component(task, cache);
            cache.emplace(key, component);
        }
        return component;
    }

    std::shared_ptr<BoundComponentType> bind(
        const std::shared_ptr<AbstractTask> &task) const {
        Cache cache;
        return bind_with_cache(task, cache);
    }
};

template<
    typename BoundComponent, typename BoundComponentType,
    typename ComponentArgs>
    requires std::derived_from<BoundComponent, BoundComponentType> &&
             BoundComponentMatchesComponentArgs<
                 BoundComponent, ComponentArgs>::value
class Component : public TypedComponent<BoundComponentType> {
    ComponentArgs args;
protected:
    virtual std::shared_ptr<BoundComponentType> create_bound_component(
        const std::shared_ptr<AbstractTask> &task,
        Cache &cache) const override {
        auto ts_args = recursively_bind_components(task, cache, args);
        return make_shared_from_tuple<BoundComponent>(task, ts_args);
    }

public:
    explicit Component(ComponentArgs &&_args)
        : TypedComponent<BoundComponentType>(
              // get description (always second to last argument)
              std::get<std::tuple_size_v<ComponentArgs> - 2>(_args),
              // get verbosity (always last argument)
              std::get<std::tuple_size_v<ComponentArgs> - 1>(_args)),
          args(move(_args)){};
};

template<typename T>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &, Cache &, const T &t) {
    return t;
}

template<typename T>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task, Cache &cache,
    const std::vector<T> &vec) {
    std::vector<decltype(recursively_bind_components(task, cache, vec[0]))>
        result;
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
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task, Cache &cache,
    const std::shared_ptr<T> &component) {
    return component->bind_with_cache(task, cache);
}

template<
    typename BoundComponent, typename BoundComponentType,
    typename ComponentArgs>
auto make_shared_component(ComponentArgs &&args) {
    static_assert(
        std::derived_from<BoundComponent, BoundComponentType>,
        "CUSTOM MESSAGE: BoundComponent must derive from BoundComponentType");
    static_assert(
        BoundComponentMatchesComponentArgs<
            BoundComponent, ComponentArgs>::value,
        "CUSTOM MESSAGE: The BoundComponent must match the Arguments");
    return make_shared<
        Component<BoundComponent, BoundComponentType, ComponentArgs>>(
        move(args));
}

#endif
