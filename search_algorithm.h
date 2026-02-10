#ifndef SEARCH_ALGORITHM_H
#define SEARCH_ALGORITHM_H

#include "component.h"

class SearchAlgorithm : public BoundComponent {
public:
    virtual void dump() = 0;
};

#endif
