#ifndef OPEN_LIST_FACTORY_H
#define OPEN_LIST_FACTORY_H

#include "component.h"
#include "open_list.h"

#include <memory>

class OpenListFactory : public BoundComponent {
public:
    OpenListFactory() = default;
    virtual ~OpenListFactory() = default;

    OpenListFactory(const OpenListFactory &) = delete;

    virtual std::unique_ptr<StateOpenList> create_state_open_list() = 0;
    virtual std::unique_ptr<EdgeOpenList> create_edge_open_list() = 0;
};

#endif
