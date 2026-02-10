#include "sum_evaluator.h"

using namespace std;

SumEvaluator::SumEvaluator(
    const std::shared_ptr<AbstractTask> &,
    const std::vector<std::shared_ptr<Evaluator>> &evals,
    const std::string &description, utils::Verbosity verboisity)
    : evals(evals) {
    std::cout << "SumEvalConstructor.cc" << std::endl;
}
