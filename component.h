#ifndef COMPONENT_H
#define COMPONENT_H

#include "component_internals.h"
#include "utils.h"

#include <memory>
#include <string>

class AbstractTask;

// common base class for all bound components (OpenLists, Evaluators, etc)
// they are *bound* to a task.
class TaskSpecificComponent {
public:
    virtual ~TaskSpecificComponent() = default;
};

// common base class for templated classes about TypedComponent and Components
class TaskIndependentComponentBase {
public:
    virtual ~TaskIndependentComponentBase() = default;
};

template<typename ComponentType>
class TaskIndependentComponent : public TaskIndependentComponentBase {
    virtual std::shared_ptr<ComponentType> create_task_specific_component(
        const std::shared_ptr<AbstractTask> &task, Cache &cache) const = 0;

public:
    std::shared_ptr<ComponentType> bind_task(
        const std::shared_ptr<AbstractTask> &task, Cache &cache) const {
        std::shared_ptr<ComponentType> component;
        const CacheKey key = std::make_pair(this, task.get());
        if (cache.count(key)) {
            std::shared_ptr<TaskSpecificComponent> entry = cache.at(key);
            component = std::dynamic_pointer_cast<ComponentType>(entry);
            assert(component);
        } else {
            component = create_task_specific_component(task, cache);
            cache.emplace(key, component);
        }
        return component;
    }

    std::shared_ptr<ComponentType> bind_task(
        const std::shared_ptr<AbstractTask> &task) const {
        Cache cache;
        return bind_task(task, cache);
    }
};

template<typename T, ComponentTypeOf<T> ComponentType, ComponentArgsFor<T> Args>
class AutoTaskIndependentComponent
    : public TaskIndependentComponent<ComponentType> {
    Args args;

    virtual std::shared_ptr<ComponentType> create_task_specific_component(
        const std::shared_ptr<AbstractTask> &task,
        Cache &cache) const override {
        auto bound_args = bind_task_recursively(args, task, cache);
        return make_shared_from_tuple<T>(task, bound_args);
    }

public:
    explicit AutoTaskIndependentComponent(Args &&args) : args(move(args)) {
    }
};

template<typename T, typename ComponentType, typename Args>
std::shared_ptr<TaskIndependentComponent<ComponentType>>
make_shared_component(Args &&args) {
    return make_shared<AutoTaskIndependentComponent<T, ComponentType, Args>>(
        move(args));
}

#endif
