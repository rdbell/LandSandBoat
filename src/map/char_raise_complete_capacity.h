#pragma once

#include <functional>

namespace charraisecompletehelpers
{

template <typename OnRaise, typename AcceptRaise, typename ResetDeathTime>
inline void Complete(OnRaise&& onRaise, AcceptRaise&& acceptRaise, ResetDeathTime&& resetDeathTime)
{
    std::invoke(onRaise);
    std::invoke(acceptRaise);
    std::invoke(resetDeathTime);
}

} // namespace charraisecompletehelpers
