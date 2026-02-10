#include "const_evaluator.h"

using namespace std;

namespace const_evaluator {
ConstEvaluator::ConstEvaluator(
    const std::shared_ptr<AbstractTask> &, int c,
    const std::string &description, utils::Verbosity verbosity)
    : c(c) {
    std::cout << "ConstEvalConstructor.cc" << std::endl;
}
}
