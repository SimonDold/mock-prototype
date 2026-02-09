#ifndef COMPONENT_H
#define COMPONENT_H

#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "hash.h"

#include "utils.h"

class AbstractTask;

// common base class for all bound components (OpneLists, Evaluators, etc)
// they are *bound* to a task.
class BoundComponent {
public:
    virtual ~BoundComponent() = default;
};


// common base class for templated classes about ComponentTypes and Components
class ComponentBase {
protected:
    const std::string description;
    const utils::Verbosity verbosity;
    // // mutable utils::LogProxy log;
    // // PlanManager plan_manager; // only used for SearchAlgorithms
public:
    ComponentBase(
        const std::string &description, utils::Verbosity verbosity)
        : description(description),
          verbosity(verbosity) {
		std::cout << " BASE " << description <<std::endl;
    }
public:
    virtual ~ComponentBase() = default;
    std::string get_description() const {
        return description;
    }
    //PlanManager &get_plan_manager() {
    //    return plan_manager;
    //}
};

using BoundComponentMap = utils::HashMap<
    const std::pair<
        const ComponentBase *,
        const std::shared_ptr<AbstractTask> *>,
    std::shared_ptr<BoundComponent>>;



template<typename... Args>
static auto bind(const std::shared_ptr<AbstractTask> &task
		 , const std::unique_ptr<BoundComponentMap> &component_map
				     , const std::tuple<Args...>& args);

template<typename Tuple>
struct BoundArgs {
    using type = decltype(bind(std::declval<std::shared_ptr<AbstractTask> >(), std::declval<std::unique_ptr<BoundComponentMap>>(), std::declval<Tuple>()));
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
    static constexpr bool value = std::constructible_from<BoundComponent, Ts...>;
};


template<typename BoundComponent, typename ComponentArgs>
struct BoundComponentMatchesComponentArgs {
    static constexpr bool value = IsConstructibleFromArgsTuple<BoundComponent, typename PrependedTuple<std::shared_ptr<AbstractTask> , typename BoundArgs<ComponentArgs>::type>::type>::value;
};


template<typename BoundComponent, typename BoundComponentArgs>
static std::shared_ptr<BoundComponent> make_shared_from_tuple(const std::shared_ptr<AbstractTask> &task, const BoundComponentArgs &args) {
    return std::apply(
        [&](auto&&... unpackedArgs) {
            return make_shared<BoundComponent>(task, std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
        },
        args
    );
}


// similar words with diffferen meaning
//
// make: forwards to make_shared
// bind: you can bind everything to a task, for some types it will make no difference (e.g. int)
//
// A component can either fetch or create its BoundComponent given a task
// create: function of the Component / ComponentType; actually computes the bound_component
// fetch: creates if not in boundcomponent map yet, otherwise reuse from BoundComponentMap


template<typename BoundComponentType>
class ComponentType : public ComponentBase {
protected:
    virtual std::shared_ptr<BoundComponentType> create_bound_component_aux(
        const std::shared_ptr<AbstractTask> &task
	 	, const std::unique_ptr<BoundComponentMap> &component_map
	) const = 0;
public :
    ComponentType(
        const std::string &description, utils::Verbosity verbosity)
        : ComponentBase(description, verbosity) {
    }
    virtual std::shared_ptr<BoundComponentType> create_bound_component(
        const std::shared_ptr<AbstractTask> &task) const = 0;
    virtual std::shared_ptr<BoundComponentType> fetch_bound_component(
        const std::shared_ptr<AbstractTask> &task,
        const std::unique_ptr<BoundComponentMap> &component_map) const = 0;
};

template<typename BoundComponent, typename BoundComponentType, typename ComponentArgs>
requires
    std::derived_from<BoundComponent, BoundComponentType>
&&
BoundComponentMatchesComponentArgs<BoundComponent, ComponentArgs>::value
class Component : public ComponentType<BoundComponentType>{
    ComponentArgs args;
    virtual std::shared_ptr<BoundComponentType> create_bound_component_aux(const std::shared_ptr<AbstractTask> &task, const std::unique_ptr<BoundComponentMap> &component_map) const override {
        std::cout << "Creating bound " << this->get_description() << "..." << std::endl;
        auto ts_args = bind(task, component_map, args);
        auto _return = make_shared_from_tuple<BoundComponent>(task, ts_args);
        std::cout << "... created bound " << this->get_description() << "." << std::endl;
        return _return;
    }
    
public:
    explicit Component(ComponentArgs &&_args)
        :ComponentType<BoundComponentType>(
              std::get<std::tuple_size<decltype(args)>::value - 2>(
                  _args), // get description (always second to last argument)
              std::get<std::tuple_size<decltype(args)>::value - 1>(
                  _args) // get verbosity (always last argument)
              )
	,args(move(_args)) {};

    
    std::shared_ptr<BoundComponentType> create_bound_component(
        const std::shared_ptr<AbstractTask> &task
	) 
	const override {
		std::cout << "Creating bound "
                     << this->get_description() << " as root component..."
                     << std::endl;
        std::unique_ptr<BoundComponentMap> component_map =
            std::make_unique<BoundComponentMap>();
	auto x = fetch_bound_component(task, component_map);
		std::cout << "... created bound"
                     << this->get_description() << " as root component!"
                     << std::endl;
        return x;//fetch_bound_component(task, component_map);
    }

    std::shared_ptr<BoundComponentType> fetch_bound_component(
        const std::shared_ptr<AbstractTask> &task
		,const std::unique_ptr<BoundComponentMap> &component_map
	) 
	const override 
	{
        std::shared_ptr<BoundComponent> component;
        const std::pair<
            const ComponentBase *,
            const std::shared_ptr<AbstractTask> *>
            key = std::make_pair(this, &task);
        if (component_map->count(key)) {
			std::cout << "Reusing bound component '"
                      << this->description
			<< "'." << std::endl;
            component = dynamic_pointer_cast<BoundComponent>(
                component_map->at(key));
        } else {
            component = dynamic_pointer_cast<BoundComponent>(
                create_bound_component_aux(
                    task, component_map
				));
            component_map->emplace(key, component);
        }
	
        return component;
    }
};

// for a type T that is neither a vector/tuple nor a Component, just return the thing.
template<typename T>
static auto bind([[maybe_unused]] const std::shared_ptr<AbstractTask> &task, [[maybe_unused]] const std::unique_ptr<BoundComponentMap> &map, const T &t) {
    return t;
}

// for a T vector, return a vector of the results from element wise bind calls
template<typename T>
static auto bind(const std::shared_ptr<AbstractTask> &task, const std::unique_ptr<BoundComponentMap> &map, const std::vector<T> &vec) {
    std::vector<decltype(bind(
        task, map, vec[0]))>
        result;
    result.reserve(vec.size());

    for (const auto &elem : vec) {
        result.push_back(
            bind(task, map, elem));
    }
    return result;
}

// for a Tuple, return a Tuple of the results from element wise bind calls
template<typename... Args>
static auto bind(const std::shared_ptr<AbstractTask> &task, 
				     const std::unique_ptr<BoundComponentMap> &map,
				     const std::tuple<Args...>& args
				     ) {
    return std::apply([&](const Args&... elems) {
        return std::make_tuple(bind(task, map, elems)...);
    }, args);
}

// for a Component, return the result of `fetch_bound_component`.
template<typename BoundComponent, typename BoundComponentType, typename ComponentArgs>
static auto bind(const std::shared_ptr<AbstractTask> &task, const std::unique_ptr<BoundComponentMap> &map, const std::shared_ptr<Component<BoundComponent, BoundComponentType, ComponentArgs>> &component) {
    return component->fetch_bound_component(task, map);
}

// for a ComponentType, return the result of `fetch_bound_component`.
template<typename T>
static auto bind(const std::shared_ptr<AbstractTask> &task,
				     const std::unique_ptr<BoundComponentMap> &map, const std::shared_ptr<ComponentType<T>> &component_type) {
    return component_type->fetch_bound_component(task, map);
}



template<typename BoundComponent, typename BoundComponentType, typename ComponentArgs>
auto make_shared_component(ComponentArgs &&args) {
    static_assert(std::derived_from<BoundComponent, BoundComponentType>,
                  "CUSTOM MESSAGE: BoundComponent must derive from BoundComponentType");
    static_assert(BoundComponentMatchesComponentArgs<BoundComponent, ComponentArgs>::value,
                  "CUSTOM MESSAGE: The BoundComponent must match the Arguments");
    auto _return =  make_shared<Component<BoundComponent, BoundComponentType, ComponentArgs>>(move(args));
return _return;
}

#endif
