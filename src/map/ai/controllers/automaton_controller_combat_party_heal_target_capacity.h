#pragma once

namespace automatoncontrollercombatpartyhealtarget
{
inline auto CanSelectTarget(bool isMaster, bool hasEnmity, int candidateEnmity, int highestEnmity, int memberHPP, int threshold, float masterDistance) -> bool
{
    return !isMaster && hasEnmity && highestEnmity < candidateEnmity && memberHPP <= threshold && masterDistance < 20.0f;
}
} // namespace automatoncontrollercombatpartyhealtarget
