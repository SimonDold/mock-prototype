#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include "state_id.h"
#include "operator_id.h"

#include <set>

class StateID;
class OperatorID;
class Evaluator;

template<class Entry>
class OpenList {
    bool only_preferred;

public:
    explicit OpenList(bool preferred_only = false);
    virtual ~OpenList() = default;
    
    virtual Entry remove_min() = 0;
    virtual bool empty() const = 0;
    virtual void clear() = 0;
    virtual void boost_preferred();
    virtual void get_path_dependent_evaluators(
        std::set<Evaluator *> &evals) = 0;
    bool only_contains_preferred_entries() const;

};

using StateOpenListEntry = StateID;
using EdgeOpenListEntry = std::pair<StateID, OperatorID>;

using StateOpenList = OpenList<StateOpenListEntry>;
using EdgeOpenList = OpenList<EdgeOpenListEntry>;

template<class Entry>
OpenList<Entry>::OpenList(bool only_preferred)
    : only_preferred(only_preferred) {
}

template<class Entry>
void OpenList<Entry>::boost_preferred() {
}


template<class Entry>
bool OpenList<Entry>::only_contains_preferred_entries() const {
    return only_preferred;
}



#endif
