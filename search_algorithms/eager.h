
#ifndef SEARCH_ALGORITHMS_EAGER_SEARCH_H
#define SEARCH_ALGORITHMS_EAGER_SEARCH_H

#include "../evaluator.h"
#include "../open_list.h"
#include "../search_algorithm.h"

#include <memory>
#include <optional>
#include <vector>

class OpenListFactory;

namespace eager_search {

class EagerSearch : public SearchAlgorithm {
    std::unique_ptr<StateOpenList> open_list;
    std::shared_ptr<Evaluator> f_evaluator;
public:
    explicit EagerSearch(
        const std::shared_ptr<AbstractTask> &,
        const std::shared_ptr<OpenListFactory> &open,
        const std::shared_ptr<Evaluator> &f_eval,
        const std::string &description, utils::Verbosity verbosity);

    void dump() override {
        std::cout << "eager" << " with:\n f-eval:" << std::endl;
        f_evaluator->dump();
    }
};
}

#endif
