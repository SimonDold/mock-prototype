#ifndef COMPONENT_H
#define COMPONENT_H

#include "hash.h"
#include "utils.h"

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
protected:
    const std::string description;
    const utils::Verbosity verbosity;
    // // mutable utils::LogProxy log;
    // // PlanManager plan_manager; // only used for SearchAlgorithms
public:
    ComponentBase(const std::string &description, utils::Verbosity verbosity)
        : description(description), verbosity(verbosity) {
    }
public:
    virtual ~ComponentBase() = default;
    std::string get_description() const {
        return description;
    }
    // PlanManager &get_plan_manager() {
    //     return plan_manager;
    // }
};

// maybe BindingMap
using BoundComponentMap = utils::HashMap<
    const std::pair<
        const ComponentBase *, const std::shared_ptr<AbstractTask> *>,
    std::shared_ptr<BoundComponent>>;

template<typename... Args>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task,
    const std::unique_ptr<BoundComponentMap> &component_map,
    const std::tuple<Args...> &args);

template<typename Tuple>
struct BoundArgs {
    using type = decltype(recursively_bind_components(
        std::declval<std::shared_ptr<AbstractTask>>(),
        std::declval<std::unique_ptr<BoundComponentMap>>(),
        std::declval<Tuple>()));
};

template<typename T, typename Tuple>
struct PrependedTuple;

template<typename T, typename... Ts>
struct PrependedTuple<T, std::tuple<Ts...>> {
    using type = std::tuple<T, Ts...>;
};

template<typename BoundComponent, typename Tuple>
struct IsConstructibleFromArgsTuple;

template<typename BoundComponent, typename... Ts>
struct IsConstructibleFromArgsTuple<BoundComponent, std::tuple<Ts...>> {
    static constexpr bool value =
        std::constructible_from<BoundComponent, Ts...>;
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
    virtual std::shared_ptr<BoundComponentType> create_bound_component(
        const std::shared_ptr<AbstractTask> &task,
        const std::unique_ptr<BoundComponentMap> &component_map) const
        = 0;

public:
    std::shared_ptr<BoundComponentType> bind_with_cache(
        const std::shared_ptr<AbstractTask> &task,
        const std::unique_ptr<BoundComponentMap> &component_map)
        const {
        std::shared_ptr<BoundComponentType> component;
        const std::pair<
            const ComponentBase *, const std::shared_ptr<AbstractTask> *>
            key = std::make_pair(this, &task);
        if (component_map->count(key)) {
            component = std::dynamic_pointer_cast<BoundComponentType>(
                component_map->at(key));
        } else {
            component = create_bound_component(task, component_map);
            component_map->emplace(key, component);
        }

        return component;
    }

    TypedComponent(const std::string &description, utils::Verbosity verbosity)
        : ComponentBase(description, verbosity) {
    }

    std::shared_ptr<BoundComponentType> bind(
        const std::shared_ptr<AbstractTask> &task) const {
        std::unique_ptr<BoundComponentMap> component_map =
            std::make_unique<BoundComponentMap>();
        return bind_with_cache(task, component_map);
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
    std::shared_ptr<BoundComponentType> create_bound_component(
        const std::shared_ptr<AbstractTask> &task,
        const std::unique_ptr<BoundComponentMap> &component_map)
        const {
        auto ts_args = recursively_bind_components(task, component_map, args);
        return make_shared_from_tuple<BoundComponent>(task, ts_args);
    }

public:
    explicit Component(ComponentArgs &&_args)
        : TypedComponent<BoundComponentType>(
              std::get<std::tuple_size<decltype(args)>::value - 2>(
                  _args), // get description (always second to last argument)
              std::get<std::tuple_size<decltype(args)>::value - 1>(
                  _args) // get verbosity (always last argument)
              ),
          args(move(_args)){};
};


// for a type T that is neither a vector/tuple nor a Component, just return the
// thing.
template<typename T>
static auto recursively_bind_components(
    [[maybe_unused]] const std::shared_ptr<AbstractTask> &task,
    [[maybe_unused]] const std::unique_ptr<BoundComponentMap> &map,
    const T &t) {
    return t;
}

// for a T vector, return a vector of the results from element wise bind calls
template<typename T>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task,
    const std::unique_ptr<BoundComponentMap> &map, const std::vector<T> &vec) {
    std::vector<decltype(recursively_bind_components(task, map, vec[0]))>
        result;
    result.reserve(vec.size());

    for (const auto &elem : vec) {
        result.push_back(recursively_bind_components(task, map, elem));
    }
    return result;
}

// for a Tuple, return a Tuple of the results from element wise bind calls
template<typename... Args>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task,
    const std::unique_ptr<BoundComponentMap> &map,
    const std::tuple<Args...> &args) {
    return std::apply(
        [&](const Args &...elems) {
            return std::make_tuple(
                recursively_bind_components(task, map, elems)...);
        },
        args);
}

// for a Component, return the result of `bind`.
template<
    typename BoundComponent, typename BoundComponentType,
    typename ComponentArgs>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task,
    const std::unique_ptr<BoundComponentMap> &map,
    const std::shared_ptr<
        Component<BoundComponent, BoundComponentType, ComponentArgs>>
        &component) {
    return component->bind_with_cache(task, map);
}

// for a TypedComponent, return the result of `bind`.
template<typename T>
static auto recursively_bind_components(
    const std::shared_ptr<AbstractTask> &task,
    const std::unique_ptr<BoundComponentMap> &map,
    const std::shared_ptr<TypedComponent<T>> &component) {
    return component->bind_with_cache(task, map);
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
    auto _return = make_shared<
        Component<BoundComponent, BoundComponentType, ComponentArgs>>(
        move(args));
    return _return;
}

#endif
