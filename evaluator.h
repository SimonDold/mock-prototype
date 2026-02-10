#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "component.h"

#include <iostream>

// fd
//
//
class Evaluator : public BoundComponent {
public:
    virtual void dump() = 0;
};

#endif
