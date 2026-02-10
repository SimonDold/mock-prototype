
#include "component.h"
#include "open_list_factory.h"

#include "evaluators/const_evaluator.h"
#include "evaluators/sum_evaluator.h"
#include "evaluators/weighted_evaluator.h"
#include "open_lists/tiebreaking_open_list.h"
#include "search_algorithms/eager.h"

#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main() {
    auto ti_c_eval = make_shared_component<const_evaluator::ConstEvaluator, Evaluator>(
        std::tuple(2, "c_eval", utils::Verbosity::NORMAL));
    auto ti_w_eval = make_shared_component<WeightedEvaluator, Evaluator>(
        std::tuple(42, ti_c_eval, "w_eval", utils::Verbosity::NORMAL));
    auto evals = std::vector<std::shared_ptr<TypedComponent<Evaluator>>>{
        ti_c_eval, ti_w_eval, ti_c_eval};
    std::shared_ptr<TypedComponent<Evaluator>> ti_sum_eval =
        make_shared_component<SumEvaluator, Evaluator>(
            std::tuple(evals, "sum_eval", utils::Verbosity::NORMAL));
    // auto w_eval = ti_w_eval->create_bound_component(AbstractTask{});
    auto w_eval =
        ti_w_eval->bind(std::shared_ptr<AbstractTask>{});
    w_eval->dump();
    std::cout << "- - - - -- " << std::endl;
    auto sum_eval =
        ti_sum_eval->bind(std::shared_ptr<AbstractTask>{});
    sum_eval->dump();

    std::cout << "- - - " << std::endl;

    std::shared_ptr<Dummy> dummy_ptr = std::make_shared<Dummy>(99);
    /*
     * not possible:
     * auto ti_dummy = make_shared_Component<Dummy>(666);
     */
    std::shared_ptr<TypedComponent<OpenListFactory>> ti_tb_olist = 
        make_shared_component<TieBreakingOpenListFactory, OpenListFactory>(
            std::tuple(evals, false, false, "tie", utils::Verbosity::NORMAL));
    auto ti_eager =
        make_shared_component<eager_search::EagerSearch, SearchAlgorithm>(
            std::tuple(
                ti_tb_olist,
                ti_sum_eval, dummy_ptr, "eager" /*1*/,
                utils::Verbosity::NORMAL));
    auto eager =
        ti_eager->bind(std::shared_ptr<AbstractTask>{});
    eager->dump();
    std::cout << "done" << std::endl;
}
