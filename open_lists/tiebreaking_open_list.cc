#include "tiebreaking_open_list.h"

#include "../evaluator.h"
#include "../open_list.h"

#include <vector>

using namespace std;

template<class Entry>
class TieBreakingOpenList : public OpenList<Entry> {
    vector<shared_ptr<Evaluator>> evaluators;
    bool allow_unsafe_pruning;


public:
    TieBreakingOpenList(
        const vector<shared_ptr<Evaluator>> &evals, bool unsafe_pruning,
        bool pref_only);

    void dump() override {
        std::cout << "TBOpenList(NOT factory) with evals:\n" << std::endl;
        for (auto eval : evaluators) {
            std::cout << "TBOL_eval: ";
            eval->dump();
            std::cout << std::endl;
        }
    }
};

template<class Entry>
TieBreakingOpenList<Entry>::TieBreakingOpenList(
    const vector<shared_ptr<Evaluator>> &evals, bool unsafe_pruning,
    bool pref_only)
    : OpenList<Entry>(pref_only),
      evaluators(evals),
      allow_unsafe_pruning(unsafe_pruning) {
    std::cout << "TieBreakingOpenList_Constructor (NOT factory)" << std::endl;
}

TieBreakingOpenListFactory::TieBreakingOpenListFactory(
    const std::shared_ptr<AbstractTask> &task,
    const std::vector<std::shared_ptr<Evaluator>> &evals, bool unsafe_pruning,
    bool pref_only, const std::string &description, utils::Verbosity verbosity)
    : evals(evals), unsafe_pruning(unsafe_pruning), pref_only(pref_only) {
    std::cout << "TieBreakingOpenListFactory_Constructor" << std::endl;
}

unique_ptr<StateOpenList> TieBreakingOpenListFactory::create_state_open_list() {
    return make_unique<TieBreakingOpenList<StateOpenListEntry>>(
        evals, unsafe_pruning, pref_only);
}

unique_ptr<EdgeOpenList> TieBreakingOpenListFactory::create_edge_open_list() {
    return make_unique<TieBreakingOpenList<EdgeOpenListEntry>>(
        evals, unsafe_pruning, pref_only);
}
