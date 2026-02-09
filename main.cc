
#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <vector>



#include "component.h"

#include "evaluators/const_evaluator.h"
#include "evaluators/sum_evaluator.h"
#include "evaluators/weighted_evaluator.h"

#include "search_algorithms/eager.h"



int main() {
    auto ti_c_eval = make_shared_Component<ConstEvaluator, Evaluator>(std::tuple(2, "c_eval", utils::Verbosity::NORMAL));
    auto ti_w_eval = make_shared_Component<WeightedEvaluator, Evaluator>(std::tuple(42, ti_c_eval, "w_eval", utils::Verbosity::NORMAL));
    auto evals = std::vector<std::shared_ptr<ComponentType<Evaluator>>>{ti_c_eval,ti_w_eval,ti_c_eval};
    std::shared_ptr<ComponentType<Evaluator>> ti_sum_eval = make_shared_Component<SumEvaluator, Evaluator>(std::tuple(evals, "sum_eval", utils::Verbosity::NORMAL));
    //auto w_eval = ti_w_eval->create_bound_component(AbstractTask{});
    auto w_eval = ti_w_eval->fetch_bound_component(std::shared_ptr<AbstractTask> {});
    w_eval->dump();
    std::cout << "- - - - -- " << std::endl;
    auto sum_eval = ti_sum_eval->fetch_bound_component(std::shared_ptr<AbstractTask> {});
    sum_eval->dump();
    
    std::cout << "- - - " << std::endl;
    
    std::shared_ptr<Dummy> dummy_ptr = std::make_shared<Dummy>(99);
    /*
     * not possible:
     * auto ti_dummy = make_shared_Component<Dummy>(666);
     */
    auto ti_eager = make_shared_Component<eager_search::EagerSearch, SearchAlgorithm>(std::tuple(ti_sum_eval, dummy_ptr, /*"eager"*/1, utils::Verbosity::NORMAL));
    auto eager = ti_eager->fetch_bound_component(std::shared_ptr<AbstractTask>{});
    eager->dump();
    std::cout << "done" << std::endl;
}
