
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
    auto ti_c_eval = make_shared_TaskIndependentComponent<ConstEvaluator, Evaluator>(std::tuple(2, "c_eval", utils::Verbosity::NORMAL));
    auto ti_w_eval = make_shared_TaskIndependentComponent<WeightedEvaluator, Evaluator>(std::tuple(42, ti_c_eval, "w_eval", utils::Verbosity::NORMAL));
	auto ints = std::vector<int>{1,2};
	auto evals = std::vector<std::shared_ptr<TaskIndependentComponentType<Evaluator>>>{ti_c_eval,ti_w_eval,ti_c_eval};
    std::shared_ptr<TaskIndependentComponentType<Evaluator>> ti_sum_eval = make_shared_TaskIndependentComponent<SumEvaluator, Evaluator>(std::tuple(evals, "sum_eval", utils::Verbosity::NORMAL));
    //auto w_eval = ti_w_eval->create_task_specific(AbstractTask{});
    auto w_eval = ti_w_eval->get_task_specific(std::shared_ptr<AbstractTask> {});
    w_eval->dump();
	std::cout << "- - - - -- " << std::endl;
    auto sum_eval = ti_sum_eval->get_task_specific(std::shared_ptr<AbstractTask> {});
    sum_eval->dump();

	std::cout << "- - - " << std::endl;

   auto ti_eager = make_shared_TaskIndependentComponent<eager_search::EagerSearch, SearchAlgorithm>(std::tuple(ti_sum_eval, "eager", utils::Verbosity::NORMAL));
   auto eager = ti_eager->get_task_specific(std::shared_ptr<AbstractTask>{});
   eager->dump();
	std::cout << "done" << std::endl;
}
