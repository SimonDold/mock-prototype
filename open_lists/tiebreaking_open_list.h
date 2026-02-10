#ifndef OPEN_LISTS_TIEBREAKING_OPEN_LIST_H
#define OPEN_LISTS_TIEBREAKING_OPEN_LIST_H

#include "../evaluator.h"
#include "../open_list_factory.h"

class TieBreakingOpenListFactory : public OpenListFactory {
    std::vector<std::shared_ptr<Evaluator>> evals;
    bool unsafe_pruning;
    bool pref_only;
public:
    TieBreakingOpenListFactory(
        const std::shared_ptr<AbstractTask> &task,
        const std::vector<std::shared_ptr<Evaluator>> &evals,
        bool unsafe_pruning, bool pref_only, const std::string &description,
        utils::Verbosity verbosity);

    virtual std::unique_ptr<StateOpenList> create_state_open_list() override;
    virtual std::unique_ptr<EdgeOpenList> create_edge_open_list() override;
};

#endif
