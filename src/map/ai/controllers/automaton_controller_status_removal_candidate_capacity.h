#pragma once

#include <algorithm>
#include <vector>

namespace automatoncontrollerstatusremovalcandidate
{
template <typename Status>
inline auto CanMatchStatusRemoval(const std::vector<Status>& removes, Status status) -> bool
{
    return std::find(removes.begin(), removes.end(), status) != removes.end();
}
} // namespace automatoncontrollerstatusremovalcandidate
