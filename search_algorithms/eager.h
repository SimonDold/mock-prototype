
#ifndef SEARCH_ALGORITHMS_EAGER_SEARCH_H
#define SEARCH_ALGORITHMS_EAGER_SEARCH_H

#include "../search_algorithm.h"

#include <memory>
#include <optional>
#include <vector>

#include "../evaluator.h"

#include "../dummy.h"

namespace eager_search {


class EagerSearch : public SearchAlgorithm {
    std::shared_ptr<Evaluator> f_evaluator;
    int number;
public:
    explicit EagerSearch(
        const std::shared_ptr<AbstractTask> &, 
        const std::shared_ptr<Evaluator> &f_eval,
        const std::shared_ptr<Dummy> &dummy,
        const std::string &description, utils::Verbosity verbosity)
	: f_evaluator(f_eval),
	  number(dummy->get_int())
	{};

    void dump() override {
    std::cout << "eager" << number << " with: " <<  std::endl;
    f_evaluator->dump() ; 
    }

};
}

#endif
