#pragma once

namespace mobcontrollertpmovedispatch
{
// CanDispatch reports whether a TP-move consumes this combat tick.
template <typename ShouldUseTPMove, typename DispatchMobSkill>
constexpr auto CanDispatch(const bool cooldownReady, ShouldUseTPMove&& shouldUseTPMove, DispatchMobSkill&& dispatchMobSkill) -> bool
{
    return cooldownReady && shouldUseTPMove() && dispatchMobSkill();
}
} // namespace mobcontrollertpmovedispatch
