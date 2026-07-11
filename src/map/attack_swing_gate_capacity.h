#pragma once

#include "common/cbasetypes.h"

namespace attackswinggatehelpers
{

enum class SwingGateOutcome : uint8
{
    PerfectDodge,
    Deflected,
    HitPath,
    Miss,
};

// ResolveSwingGate preserves OnAttack's exclusive ladder and avoids hit-roll
// or All Miss evaluation when an earlier outcome wins.
template <typename HitCandidate, typename HasAllMiss>
inline auto ResolveSwingGate(
    const bool perfectDodge,
    const bool deflected,
    HitCandidate&& hitCandidate,
    HasAllMiss&& hasAllMiss) -> SwingGateOutcome
{
    if (perfectDodge)
    {
        return SwingGateOutcome::PerfectDodge;
    }
    if (deflected)
    {
        return SwingGateOutcome::Deflected;
    }
    if (hitCandidate() && !hasAllMiss())
    {
        return SwingGateOutcome::HitPath;
    }
    return SwingGateOutcome::Miss;
}

} // namespace attackswinggatehelpers
