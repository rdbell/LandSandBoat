#pragma once

namespace automatoncontrollerdiabiopriority
{
inline auto CanPrioritize(bool opposingEffectMissing, bool correspondingManeuver) -> bool
{
    return opposingEffectMissing && correspondingManeuver;
}
} // namespace automatoncontrollerdiabiopriority
