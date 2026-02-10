#ifndef EVALUATORS_CONST_EVALUATOR_H
#define EVALUATORS_CONST_EVALUATOR_H
#include "../evaluator.h"

namespace const_evaluator {
class ConstEvaluator : public Evaluator {
    int c;
public:
    ConstEvaluator(
        const std::shared_ptr<AbstractTask> &, int c,
        const std::string &description, utils::Verbosity);

    void dump() override {
        std::cout << c << std::endl;
    }
};
}
#endif
