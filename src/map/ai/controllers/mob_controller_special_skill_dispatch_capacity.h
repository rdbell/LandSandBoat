#pragma once

namespace mobcontrollerspecialskilldispatch
{
// CanDispatch reports whether a special skill consumes this combat tick.
template <typename DispatchSpecialSkill>
constexpr auto CanDispatch(const bool ready, DispatchSpecialSkill&& dispatchSpecialSkill) -> bool
{
    return ready && dispatchSpecialSkill();
}
} // namespace mobcontrollerspecialskilldispatch
