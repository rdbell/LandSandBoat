#pragma once

#include "common/cbasetypes.h"

namespace attackhitpathhelpers
{

enum class HitPathOutcome : uint8
{
    Parried,
    ShadowAbsorbed,
    Reactive,
    Landed,
};

// ResolveHitPath preserves the nested OnAttack precedence and callback
// short-circuiting. Reactive intentionally does not distinguish anticipation
// from counter: CheckAnticipated may set either flag and returns Lua validity.
template <typename CheckParried, typename SATAOccurred, typename AbsorbShadow, typename CheckAnticipated, typename CheckCounter>
inline auto ResolveHitPath(
    CheckParried&& checkParried,
    SATAOccurred&& sataOccurred,
    AbsorbShadow&& absorbShadow,
    CheckAnticipated&& checkAnticipated,
    CheckCounter&& checkCounter) -> HitPathOutcome
{
    if (checkParried())
    {
        return HitPathOutcome::Parried;
    }
    if (!sataOccurred() && absorbShadow())
    {
        return HitPathOutcome::ShadowAbsorbed;
    }
    if (checkAnticipated() || checkCounter())
    {
        return HitPathOutcome::Reactive;
    }
    return HitPathOutcome::Landed;
}

} // namespace attackhitpathhelpers
