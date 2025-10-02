#ifndef EVALUATORS_SUM_EVALUATOR_H
#define EVALUATORS_SUM_EVALUATOR_H


#include "../evaluator.h"


class SumEvaluator: public Evaluator {
	std::vector<std::shared_ptr<Evaluator>> evals;
public:
    SumEvaluator(const std::shared_ptr<AbstractTask> &, const std::vector<std::shared_ptr<Evaluator>> &evals, std::string, utils::Verbosity): evals(evals) {
		std::cout << "SumEvalConstructor" << std::endl;
	}

    void dump() override {
		for(auto eval : evals){
			std::cout << " +++ ";
                    eval->dump();
			std::cout << std::endl;

		}
    }
};

#endif
