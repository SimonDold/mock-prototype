#include "tiebreaking_open_list.h"

#include "../evaluator.h"
#include "../open_list.h"

#include <cassert>
#include <deque>
#include <map>
#include <vector>

using namespace std;

TieBreakingOpenListFactory::TieBreakingOpenListFactory(
    const std::shared_ptr<AbstractTask> &task,
    const std::vector<std::shared_ptr<Evaluator>> &evals,
    const std::string &description, utils::Verbosity verbosity)
    : evals(evals) {
    std::cout << "TieBreakingOpenListFactory_Constructor" << std::endl;
}

unique_ptr<StateOpenList> TieBreakingOpenListFactory::create_state_open_list() {
    return nullptr;
}

unique_ptr<EdgeOpenList> TieBreakingOpenListFactory::create_edge_open_list() {
    return nullptr;
}
