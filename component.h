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

template<typename Category>
class TypedComponent : public ComponentBase {
protected:
    const std::string description;
    const utils::Verbosity verbosity;
    // // mutable utils::LogProxy log;

    virtual std::shared_ptr<Category> create_bound_component(
        const std::shared_ptr<AbstractTask> &task, Cache &cache) const = 0;

public:
    TypedComponent(const std::string &description, utils::Verbosity verbosity)
        : description(description), verbosity(verbosity) {
    }

    std::shared_ptr<Category> bind_with_cache(
        const std::shared_ptr<AbstractTask> &task, Cache &cache) const {
        std::shared_ptr<Category> component;
        const CacheKey key = std::make_pair(this, task.get());
        if (cache.count(key)) {
            std::shared_ptr<BoundComponent> entry = cache.at(key);
            component = std::dynamic_pointer_cast<Category>(entry);
            assert(component);
        } else {
            component = create_bound_component(task, cache);
            cache.emplace(key, component);
        }
        return component;
    }

    std::shared_ptr<Category> bind(
        const std::shared_ptr<AbstractTask> &task) const {
        Cache cache;
        return bind_with_cache(task, cache);
    }
};

template<typename T, ComponentCategoryOf<T> Category, ComponentArgsFor<T> Args>
class Component : public TypedComponent<Category> {
    Args args;

protected:
    virtual std::shared_ptr<Category> create_bound_component(
        const std::shared_ptr<AbstractTask> &task,
        Cache &cache) const override {
        auto bound_args = recursively_bind_components(task, cache, args);
        return make_shared_from_tuple<T>(task, bound_args);
    }

public:
    explicit Component(Args &&args)
        : TypedComponent<Category>(
              // get description (always second to last argument)
              std::get<std::tuple_size_v<Args> - 2>(args),
              // get verbosity (always last argument)
              std::get<std::tuple_size_v<Args> - 1>(args)),
          args(move(args)){};
};

template<typename T, typename Category, typename Args>
std::shared_ptr<TypedComponent<Category>> make_shared_component(Args &&args) {
    return make_shared<Component<T, Category, Args>>(move(args));
}

#endif
