#include "tiebreaking_open_list.h"

#include "../evaluator.h"
#include "../open_list.h"

#include <cassert>
#include <deque>
#include <map>
#include <vector>

using namespace std;

template<class Entry>
class TieBreakingOpenList : public OpenList<Entry> {
    using Bucket = deque<Entry>;

    map<const vector<int>, Bucket> buckets;
    int size;

    vector<shared_ptr<Evaluator>> evaluators;
    /*
      If allow_unsafe_pruning is true, we ignore (don't insert) states
      which the first evaluator considers a dead end, even if it is
      not a safe heuristic.
    */
    bool allow_unsafe_pruning;

    int dimension() const;


public:
    TieBreakingOpenList(
        const vector<shared_ptr<Evaluator>> &evals, bool unsafe_pruning,
        bool pref_only);

    virtual Entry remove_min() override;
    virtual bool empty() const override;
    virtual void clear() override;
    virtual void get_path_dependent_evaluators(
        set<Evaluator *> &evals) override;
};

template<class Entry>
TieBreakingOpenList<Entry>::TieBreakingOpenList(
    const vector<shared_ptr<Evaluator>> &evals, bool unsafe_pruning,
    bool pref_only)
    : OpenList<Entry>(pref_only),
      size(0),
      evaluators(evals),
      allow_unsafe_pruning(unsafe_pruning) {
}

template<class Entry>
Entry TieBreakingOpenList<Entry>::remove_min() {
    assert(size > 0);
    typename map<const vector<int>, Bucket>::iterator it;
    it = buckets.begin();
    assert(it != buckets.end());
    assert(!it->second.empty());
    --size;
    Entry result = it->second.front();
    it->second.pop_front();
    if (it->second.empty())
        buckets.erase(it);
    return result;
}

template<class Entry>
bool TieBreakingOpenList<Entry>::empty() const {
    return size == 0;
}

template<class Entry>
void TieBreakingOpenList<Entry>::clear() {
    buckets.clear();
    size = 0;
}

template<class Entry>
int TieBreakingOpenList<Entry>::dimension() const {
    return evaluators.size();
}

template<class Entry>
void TieBreakingOpenList<Entry>::get_path_dependent_evaluators(
    set<Evaluator *> &evals) {
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
