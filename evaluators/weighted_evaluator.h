#ifndef EVALUATORS_WEIGHTED_EVALUATOR_H
#define EVALUATORS_WEIGHTED_EVALUATOR_H

#include "../evaluator.h"

class WeightedEvaluator: public Evaluator {
    int w;
	std::shared_ptr<Evaluator> eval;
public:
    WeightedEvaluator(const std::shared_ptr<AbstractTask> &, int w, const std::shared_ptr<Evaluator> &eval, std::string, utils::Verbosity): w(w), eval(eval) {
		std::cout << "WeightedEvalConstructor" << std::endl;
}
    void dump() override {
		std::cout << w << " * ";
        eval->dump();
		std::cout << std::endl;
    }
};

#endif
