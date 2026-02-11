#ifndef SEARCH_ALGORITHM_H
#define SEARCH_ALGORITHM_H

#include "component.h"

#include <iostream>

class SearchAlgorithm : public TaskSpecificComponent {
public:
    virtual void dump() = 0;
};

#endif
