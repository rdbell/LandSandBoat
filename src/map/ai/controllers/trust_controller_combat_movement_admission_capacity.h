#pragma once

namespace trustcontrollercombatmovementadmission
{
// CanMove reports whether a trust is free to run combat movement logic.
template <typename IsInMagicState, typename IsInRangeState>
constexpr auto CanMove(IsInMagicState&& isInMagicState, IsInRangeState&& isInRangeState) -> bool
{
    return !isInMagicState() && !isInRangeState();
}
} // namespace trustcontrollercombatmovementadmission
