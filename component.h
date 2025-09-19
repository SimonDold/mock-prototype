
#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "hash.h"

#include "utils.h"

class AbstractTask;

// common base class for all components (OpneLists, Evaluators, etc)
class Component {
public:
    virtual ~Component() = default;
};


// common base class for templated classes about TaskIndependentTypes and TaskIndependentComponents
class TaskIndependentBase {
protected:
    const std::string description;
    const utils::Verbosity verbosity;
    // // mutable utils::LogProxy log;
    // // PlanManager plan_manager; // only used for SearchAlgorithms
public:
    TaskIndependentBase(
        const std::string &description, utils::Verbosity verbosity)
        : description(description),
          verbosity(verbosity) {
		std::cout << " BASE " << description <<std::endl;
    }
public:
    virtual ~TaskIndependentBase() = default;
    std::string get_description() const {
        return description;
    }
    //PlanManager &get_plan_manager() {
    //    return plan_manager;
    //}
};

using ComponentMap = utils::HashMap<
    const std::pair<
        const TaskIndependentBase *,
        const std::shared_ptr<AbstractTask> *>,
    std::shared_ptr<Component>>;



template<typename... Args>
static auto make_task_specific_tuple(const std::shared_ptr<AbstractTask> &task
		 , const std::unique_ptr<ComponentMap> &component_map
				     , const std::tuple<Args...>& args);

template<typename Tuple>
struct TaskSpecifiedArgs {
    using type = decltype(make_task_specific_tuple(std::declval<std::shared_ptr<AbstractTask> >(), std::declval<std::unique_ptr<ComponentMap>>(), std::declval<Tuple>()));
};


template<typename T, typename Tuple>
struct PrependedTuple;

template<typename T, typename... Ts>
struct PrependedTuple<T, std::tuple<Ts...>> {
    using type = std::tuple<T, Ts...>;
};


template<typename Component, typename Tuple>
struct IsConstructibleFromArgsTuple;

template<typename Component, typename... Ts>
struct IsConstructibleFromArgsTuple<Component, std::tuple<Ts...>> {
    static constexpr bool value = std::constructible_from<Component, Ts...>;
};








template<typename Component, typename TIComponentArgs>
struct ComponentMatchesTIComponentArgs {
    static constexpr bool value = IsConstructibleFromArgsTuple<Component, typename PrependedTuple<std::shared_ptr<AbstractTask> , typename TaskSpecifiedArgs<TIComponentArgs>::type>::type>::value;
};


template<typename Component, typename ComponentArgs>
static std::shared_ptr<Component> make_shared_from_tuple(const std::shared_ptr<AbstractTask> &task, const ComponentArgs &args) {
    return std::apply(
        [&](auto&&... unpackedArgs) {
            return make_shared<Component>(task, std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
        },
        args
    );
}





template<typename ComponentType>
class TaskIndependentComponentType : public TaskIndependentBase {
protected:
    virtual std::shared_ptr<ComponentType> create_task_specific(
        const std::shared_ptr<AbstractTask> &task
	 	, const std::unique_ptr<ComponentMap> &component_map
	) const = 0;
public :
    TaskIndependentComponentType(
        const std::string &description, utils::Verbosity verbosity)
        : TaskIndependentBase(description, verbosity) {
    }
    virtual std::shared_ptr<ComponentType> get_task_specific(
        const std::shared_ptr<AbstractTask> &task) const = 0;
    virtual std::shared_ptr<ComponentType> get_task_specific(
        const std::shared_ptr<AbstractTask> &task,
        const std::unique_ptr<ComponentMap> &component_map) const = 0;
};

template<typename Component, typename ComponentType, typename TIComponentArgs>
requires
    std::derived_from<Component, ComponentType>
&&
ComponentMatchesTIComponentArgs<Component, TIComponentArgs>::value
class TaskIndependentComponent : public TaskIndependentComponentType<ComponentType>{
    TIComponentArgs args;
    virtual std::shared_ptr<ComponentType> create_task_specific(const std::shared_ptr<AbstractTask> &task, const std::unique_ptr<ComponentMap> &component_map) const override {
        auto ts_args = make_task_specific_tuple(task, component_map, args);
        return make_shared_from_tuple<Component>(task, ts_args);
    }
    
public:
    explicit TaskIndependentComponent(TIComponentArgs &&_args)
        :TaskIndependentComponentType<ComponentType>(
              std::get<std::tuple_size<decltype(args)>::value - 2>(
                  _args), // get description (always second to last argument)
              std::get<std::tuple_size<decltype(args)>::value - 1>(
                  _args) // get verbosity (always last argument)
              )
	,args(move(_args)) {};

    
    std::shared_ptr<ComponentType> get_task_specific(
        const std::shared_ptr<AbstractTask> &task
	) 
	const override {
		std::cout << "Creating "
                     << this->get_description() << " as root component..."
                     << std::endl;
        std::unique_ptr<ComponentMap> component_map =
            std::make_unique<ComponentMap>();
        return get_task_specific(task, component_map);
    }

    std::shared_ptr<ComponentType> get_task_specific(
        const std::shared_ptr<AbstractTask> &task
		,const std::unique_ptr<ComponentMap> &component_map
	) 
	const override 
	{
        std::shared_ptr<Component> component;
        const std::pair<
            const TaskIndependentBase *,
            const std::shared_ptr<AbstractTask> *>
            key = std::make_pair(this, &task);
        if (component_map->count(key)) {
			std::cout << "Reusing task specific component '"
                      << this->description
			<< "'..." << std::endl;
            component = dynamic_pointer_cast<Component>(
                component_map->at(key));
        } else {
			std::cout
                      << "Creating task specific component '"
                      << this->description 
			<< "'..." << std::endl;
            component = dynamic_pointer_cast<Component>(
                create_task_specific(
                    task, component_map
				));
            component_map->emplace(key, component);
        }
        return component;
    }
};


template<typename T>
static auto make_task_specific(const std::shared_ptr<AbstractTask> &task, const std::unique_ptr<ComponentMap> &map, const T &t) {
    return t;
}

template<typename T>
static auto make_task_specific(const std::shared_ptr<AbstractTask> &task, const std::unique_ptr<ComponentMap> &map, const std::vector<T> &vec) {
    std::vector<decltype(make_task_specific(
        task, map, vec[0]))>
        result;
    result.reserve(vec.size());

    for (const auto &elem : vec) {
        result.push_back(
            make_task_specific(task, map, elem));
    }
    return result;
}

template<typename T1, typename T2, typename T3>
static auto make_task_specific(const std::shared_ptr<AbstractTask> &task, const std::unique_ptr<ComponentMap> &map, const std::shared_ptr<TaskIndependentComponent<T1, T2, T3>> &t) {
    return t->get_task_specific(task, map);
}

template<typename T>
static auto make_task_specific(const std::shared_ptr<AbstractTask> &task,
				     const std::unique_ptr<ComponentMap> &map, const std::shared_ptr<TaskIndependentComponentType<T>> &t) {
    return t->get_task_specific(task, map);
}

template<typename... Args>
static auto make_task_specific_tuple(const std::shared_ptr<AbstractTask> &task, 
				     const std::unique_ptr<ComponentMap> &map,
				     const std::tuple<Args...>& args
				     ) {
    return std::apply([&](const Args&... elems) {
        return std::make_tuple(make_task_specific(task, map, elems)...);
    }, args);
}


template<typename Component, typename ComponentType, typename TIComponentArgs>
auto make_shared_TaskIndependentComponent(TIComponentArgs &&args) {
    static_assert(std::derived_from<Component, ComponentType>,
                  "Component must derive from ComponentType");
    static_assert(ComponentMatchesTIComponentArgs<Component, TIComponentArgs>::value,
                  "The Component must match the Arguments");
    auto _return =  make_shared<TaskIndependentComponent<Component, ComponentType, TIComponentArgs>>(move(args));
	std::cout << "mTIC" << std::endl;
return _return;
}

