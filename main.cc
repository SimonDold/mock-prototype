#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class AbstractTask {
};

class Component {
};

class Evaluator : public Component {
public:
    virtual void dump() = 0;
};

class ConstEvaluator: public Evaluator {
    int c;
public:
    ConstEvaluator(const AbstractTask &, int c): c(c) {
cout << "ConstEvalConstructor" << endl;
	}

    void dump() override {
        cout << c << endl;
    }
};

class WeightedEvaluator: public Evaluator {
    int w;
    shared_ptr<Evaluator> eval;
public:
    WeightedEvaluator(const AbstractTask &, int w, const shared_ptr<Evaluator> &eval): w(w), eval(eval) {}

    void dump() override {
        cout << w << " * ";
        eval->dump();
        cout << endl;
    }
};

class SumEvaluator: public Evaluator {
    vector<shared_ptr<Evaluator>> evals;
public:
    SumEvaluator(const AbstractTask &, const vector<shared_ptr<Evaluator>> &evals): evals(evals) {}

    void dump() override {
		for(auto eval : evals){
                    cout << " + ";
                    eval->dump();
                    cout << endl;

		}
    }
};

template<typename... Args>
static auto make_task_specific_tuple(const AbstractTask &task, const std::tuple<Args...>& args);

template<typename Tuple>
struct TaskSpecifiedArgs {
    using type = decltype(make_task_specific_tuple(std::declval<AbstractTask>(), std::declval<Tuple>()));
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
    static constexpr bool value = IsConstructibleFromArgsTuple<Component, typename PrependedTuple<AbstractTask, typename TaskSpecifiedArgs<TIComponentArgs>::type>::type>::value;
};


template<typename Component, typename ComponentArgs>
static shared_ptr<Component> make_shared_from_tuple(const AbstractTask &task, const ComponentArgs &args) {
    return std::apply(
        [&](auto&&... unpackedArgs) {
            return make_shared<Component>(task, std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
        },
        args
    );
}


template<typename ComponentType>
class TaskIndependentComponentType {

public :
	virtual shared_ptr<ComponentType> create_task_specific(const AbstractTask &task) = 0;
};

template<typename Component, typename ComponentType, typename TIComponentArgs>
requires
    derived_from<Component, ComponentType>
&&
ComponentMatchesTIComponentArgs<Component, TIComponentArgs>::value
class TaskIndependentComponent : public TaskIndependentComponentType<ComponentType>{
    TIComponentArgs args;
    
public:
    TaskIndependentComponent(TIComponentArgs &&args)
        :args(move(args)) {};

    
    shared_ptr<ComponentType> create_task_specific(const AbstractTask &task) override {
        auto ts_args = make_task_specific_tuple(task, args);
        return make_shared_from_tuple<Component>(task, ts_args);
    }

    std::shared_ptr<ComponentType> get_task_specific(
        const AbstractTask &task
	//	,std::unique_ptr<ComponentMap> &component_map
	) 
	//const override 
	{
        std::shared_ptr<Component> component;
        // needs componentMap // const std::pair<
        // needs componentMap //     const TaskIndependentComponentBase *,
        // needs componentMap //     const std::shared_ptr<AbstractTask> *>
        // needs componentMap //     key = std::make_pair(this, &task);
        // needs componentMap // if (component_map->count(key)) {
        // needs componentMap //     cout << "Reusing task specific component '"
        // needs componentMap //               //<< this->description
	// needs componentMap // 		<< "'..." << std::endl;
        // needs componentMap //     component = dynamic_pointer_cast<ComponentT>(
        // needs componentMap //         component_map->at(key));
        // needs componentMap // } else {
            cout
                      << "Creating task specific component '"
                      //<< this->description 
			<< "'..." << std::endl;
            component = dynamic_pointer_cast<Component>(
                create_task_specific(
                    task// needs componentMAp //, component_map
				));
            // needs ComponentMap // component_map->emplace(key, component);
        // nedds ComponentMAp //}
        return component;
    }
};


template<typename T>
static auto make_task_specific(const AbstractTask &task, const T &t) {
    return t;
}

template<typename T>
static auto make_task_specific(const AbstractTask &task, const std::vector<T> &vec) {
    std::vector<decltype(make_task_specific(
        task, vec[0]))>
        result;
    result.reserve(vec.size());

    for (const auto &elem : vec) {
        result.push_back(
            make_task_specific(task, elem));
    }
    return result;
}

template<typename T1, typename T2, typename T3>
static auto make_task_specific(const AbstractTask &task, const shared_ptr<TaskIndependentComponent<T1, T2, T3>> &t) {
    return t->create_task_specific(task);
}

template<typename T>
static auto make_task_specific(const AbstractTask &task, const shared_ptr<TaskIndependentComponentType<T>> &t) {
    return t->create_task_specific(task);
}

template<typename... Args>
static auto make_task_specific_tuple(const AbstractTask &task, const std::tuple<Args...>& args) {
    return std::apply([&](const Args&... elems) {
        return std::make_tuple(make_task_specific(task, elems)...);
    }, args);
}


template<typename Component, typename ComponentType, typename TIComponentArgs>
auto makeTaskIndependentComponent(TIComponentArgs &&args) {
    static_assert(std::derived_from<Component, ComponentType>,
                  "Component must derive from ComponentType");
    static_assert(ComponentMatchesTIComponentArgs<Component, TIComponentArgs>::value,
                  "The Component must match the Arguments");
    auto _return =  make_shared<TaskIndependentComponent<Component, ComponentType, TIComponentArgs>>(move(args));
	cout << "mTIC" << endl;
return _return;
}

int main() {
    auto ti_c_eval = makeTaskIndependentComponent<ConstEvaluator, Evaluator>(tuple(2));
    auto ti_w_eval = makeTaskIndependentComponent<WeightedEvaluator, Evaluator>(tuple(42, ti_c_eval));
	auto ints = vector<int>{1,2};
	auto evals = vector<shared_ptr<TaskIndependentComponentType<Evaluator>>>{ti_c_eval,ti_w_eval};
    auto ti_sum_eval = makeTaskIndependentComponent<SumEvaluator, Evaluator>(tuple(evals));
    //auto w_eval = ti_w_eval->create_task_specific(AbstractTask{});
    auto w_eval = ti_w_eval->get_task_specific(AbstractTask{});
    w_eval->dump();
    auto sum_eval = ti_sum_eval->get_task_specific(AbstractTask{});
    sum_eval->dump();
    cout << "done" << endl;
}
