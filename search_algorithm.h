#ifndef SEARCH_ALGORITHM_H
#define SEARCH_ALGORITHM_H

#include "component.h"
#include "utils/logging.h"

#include <iostream>

class SearchAlgorithm : public TaskSpecificComponent {
public:
    SearchAlgorithm(const std::shared_ptr<AbstractTask> &task)
    : TaskSpecificComponent(task) {
    }
    virtual void dump() = 0;
};

#endif
