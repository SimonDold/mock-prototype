#ifndef PLUGINS_PLUGIN_H
#define PLUGINS_PLUGIN_H

#include "../utils/tuples.h"

#include <memory>

namespace plugins {
    template<typename T, typename... Arguments>
std::shared_ptr<T> make_shared_from_arg_tuples(Arguments... arguments) {
    return std::apply(
        [](auto &&...flattened_args) {
            return std::make_shared<T>(
                std::forward<decltype(flattened_args)>(flattened_args)...);
        },
        utils::flatten_tuple(
            std::tuple<Arguments...>(std::forward<Arguments>(arguments)...)));

}
}

#endif