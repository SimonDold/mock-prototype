
#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <vector>


#include "component.h"

// fd
//
//
class Evaluator : public Component {
public:
    virtual void dump() = 0;
};

class ConstEvaluator: public Evaluator {
    int c;
public:
    ConstEvaluator(const std::shared_ptr<AbstractTask> &, int c, std::string, utils::Verbosity): c(c) {
		std::cout << "ConstEvalConstructor" << std::endl;
	}

    void dump() override {
		std::cout << c << std::endl;
    }
};

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

class SumEvaluator: public Evaluator {
	std::vector<std::shared_ptr<Evaluator>> evals;
public:
    SumEvaluator(const std::shared_ptr<AbstractTask> &, const std::vector<std::shared_ptr<Evaluator>> &evals, std::string, utils::Verbosity): evals(evals) {
		std::cout << "SumEvalConstructor" << std::endl;
	}

    void dump() override {
		for(auto eval : evals){
			std::cout << " + ";
                    eval->dump();
			std::cout << std::endl;

		}
    }
};


int main() {
    auto ti_c_eval = make_shared_TaskIndependentComponent<ConstEvaluator, Evaluator>(std::tuple(2, "c_eval", utils::Verbosity::NORMAL));
    auto ti_w_eval = make_shared_TaskIndependentComponent<WeightedEvaluator, Evaluator>(std::tuple(42, ti_c_eval, "w_eval", utils::Verbosity::NORMAL));
	auto ints = std::vector<int>{1,2};
	auto evals = std::vector<std::shared_ptr<TaskIndependentComponentType<Evaluator>>>{ti_c_eval,ti_w_eval,ti_c_eval};
    auto ti_sum_eval = make_shared_TaskIndependentComponent<SumEvaluator, Evaluator>(std::tuple(evals, "sum_eval", utils::Verbosity::NORMAL));
    //auto w_eval = ti_w_eval->create_task_specific(AbstractTask{});
    auto w_eval = ti_w_eval->get_task_specific(std::shared_ptr<AbstractTask> {});
    w_eval->dump();
	std::cout << "- - - - -- " << std::endl;
    auto sum_eval = ti_sum_eval->get_task_specific(std::shared_ptr<AbstractTask> {});
    sum_eval->dump();
	std::cout << "done" << std::endl;
}
