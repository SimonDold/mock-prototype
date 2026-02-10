#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include "operator_id.h"
#include "state_id.h"

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

    virtual void dump() = 0;
};

using StateOpenListEntry = StateID;
using EdgeOpenListEntry = std::pair<StateID, OperatorID>;

using StateOpenList = OpenList<StateOpenListEntry>;
using EdgeOpenList = OpenList<EdgeOpenListEntry>;

template<class Entry>
OpenList<Entry>::OpenList(bool only_preferred)
    : only_preferred(only_preferred) {
}

#endif
