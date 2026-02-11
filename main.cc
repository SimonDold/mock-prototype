#include "component.h"
#include "open_list_factory.h"

#include "evaluators/const_evaluator.h"
#include "evaluators/sum_evaluator.h"
#include "evaluators/weighted_evaluator.h"
#include "open_lists/tiebreaking_open_list.h"
#include "search_algorithms/eager.h"

#include <iostream>
#include <memory>
#include <vector>

using namespace std;

int main() {
    using EvaluatorComponent = shared_ptr<TypedComponent<Evaluator>>;
    using OpenListComponent = shared_ptr<TypedComponent<OpenListFactory>>;
    using SearchComponent = shared_ptr<TypedComponent<SearchAlgorithm>>;

    EvaluatorComponent c_eval =
        make_shared_component<const_evaluator::ConstEvaluator, Evaluator>(
            tuple(2, "c_eval", utils::Verbosity::NORMAL));
    EvaluatorComponent w_eval =
        make_shared_component<WeightedEvaluator, Evaluator>(
            tuple(42, c_eval, "w_eval", utils::Verbosity::NORMAL));
    vector<EvaluatorComponent> evals{c_eval, w_eval, c_eval};
    EvaluatorComponent sum_eval =
        make_shared_component<SumEvaluator, Evaluator>(
            tuple(evals, "sum_eval", utils::Verbosity::NORMAL));

    shared_ptr<AbstractTask> task;
    shared_ptr<Evaluator> bound_w_eval = w_eval->bind(task);
    bound_w_eval->dump();
    cout << "- - - - -- " << endl;
    shared_ptr<Evaluator> bound_sum_eval = sum_eval->bind(task);
    bound_sum_eval->dump();

    cout << "- - - " << endl;

    OpenListComponent tb_olist =
        make_shared_component<TieBreakingOpenListFactory, OpenListFactory>(
            tuple(evals, false, false, "tie", utils::Verbosity::NORMAL));
    SearchComponent eager =
        make_shared_component<eager_search::EagerSearch, SearchAlgorithm>(
            tuple(tb_olist, sum_eval, "eager" /*1*/, utils::Verbosity::NORMAL));
    shared_ptr<SearchAlgorithm> bound_eager = eager->bind(task);
    bound_eager->dump();
    cout << "done" << endl;
}
