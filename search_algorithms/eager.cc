
#include "eager.h"

#include "../evaluator.h"
#include "../open_list_factory.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <optional>
#include <set>

using namespace std;

namespace eager_search {
EagerSearch::EagerSearch(
    const std::shared_ptr<AbstractTask> &task,
    const shared_ptr<OpenListFactory> &open,
    const shared_ptr<Evaluator> &f_eval, const string &description,
    utils::Verbosity verbosity)
    : SearchAlgorithm(task), f_evaluator(f_eval), open_list(open->create_state_open_list()){};
}
