#pragma once

namespace automatoncontrollernoncombatpartyhealtarget
{
inline auto CanSelectTarget(bool isMaster, int memberHPP, int threshold, float masterDistance) -> bool
{
    return !isMaster && memberHPP <= threshold && masterDistance < 20.0f;
}
} // namespace automatoncontrollernoncombatpartyhealtarget
