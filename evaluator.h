#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "component.h"

#include "utils/logging.h"
#include <iostream>

// fd
//
//
class Evaluator : public TaskSpecificComponent {
public:
    Evaluator(const std::shared_ptr<AbstractTask> &task)
    : TaskSpecificComponent(task) {
    }

    virtual void dump() = 0;
};

#endif
