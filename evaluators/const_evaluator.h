#ifndef EVALUATORS_CONST_EVALUATOR_H
#define EVALUATORS_CONST_EVALUATOR_H
#include "../evaluator.h"

class ConstEvaluator : public Evaluator {
    int c;
public:
    ConstEvaluator(
        const std::shared_ptr<AbstractTask> &, int c, std::string,
        utils::Verbosity)
        : c(c) {
        std::cout << "ConstEvalConstructor" << std::endl;
    }

    void dump() override {
        std::cout << c << std::endl;
    }
};
#endif
