#ifndef COMPONENT_H
#define COMPONENT_H

#include "component_internals.h"
#include "utils.h"

#include <memory>
#include <string>

class AbstractTask;

// common base class for all bound components (OpenLists, Evaluators, etc)
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

template<typename BoundComponentType>
class TypedComponent : public ComponentBase {
protected:
    const std::string description;
    const utils::Verbosity verbosity;
    // // mutable utils::LogProxy log;

    virtual std::shared_ptr<BoundComponentType> create_bound_component(
        const std::shared_ptr<AbstractTask> &task, Cache &cache) const = 0;

public:
    TypedComponent(const std::string &description, utils::Verbosity verbosity)
        : description(description), verbosity(verbosity) {
    }

    std::shared_ptr<BoundComponentType> bind_with_cache(
        const std::shared_ptr<AbstractTask> &task, Cache &cache) const {
        std::shared_ptr<BoundComponentType> component;
        const CacheKey key = std::make_pair(this, task.get());
        if (cache.count(key)) {
            std::shared_ptr<BoundComponent> entry = cache.at(key);
            component = std::dynamic_pointer_cast<BoundComponentType>(entry);
            assert(component);
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
    typename BoundComponent,
    ComponentCategoryOf<BoundComponent> BoundComponentType,
    ComponentArgsFor<BoundComponent> Args>
class Component : public TypedComponent<BoundComponentType> {
    Args args;
protected:
    virtual std::shared_ptr<BoundComponentType> create_bound_component(
        const std::shared_ptr<AbstractTask> &task,
        Cache &cache) const override {
        auto ts_args = recursively_bind_components(task, cache, args);
        return make_shared_from_tuple<BoundComponent>(task, ts_args);
    }

public:
    explicit Component(Args &&_args)
        : TypedComponent<BoundComponentType>(
              // get description (always second to last argument)
              std::get<std::tuple_size_v<Args> - 2>(_args),
              // get verbosity (always last argument)
              std::get<std::tuple_size_v<Args> - 1>(_args)),
          args(move(_args)){};
};

template<
    typename BoundComponent, typename BoundComponentType,
    typename ComponentArgs>
std::shared_ptr<TypedComponent<BoundComponentType>>
make_shared_component(ComponentArgs &&args) {
    return make_shared<
        Component<BoundComponent, BoundComponentType, ComponentArgs>>(
        move(args));
}

#endif
