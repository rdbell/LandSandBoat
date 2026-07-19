#pragma once

namespace mobcontrollerspelldispatch
{
// CanDispatch reports whether a spell cast consumes this combat tick.
template <typename DispatchSpell>
constexpr auto CanDispatch(const bool ready, DispatchSpell&& dispatchSpell) -> bool
{
    return ready && dispatchSpell();
}
} // namespace mobcontrollerspelldispatch
