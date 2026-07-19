#pragma once

namespace automatoncontrollerenhancementselftarget
{
inline auto CanSelectTarget(bool hasTarget, bool hasEnhancement) -> bool
{
    return !hasTarget && !hasEnhancement;
}

inline auto CanSelectSelf(bool hasTarget, bool hasEnhancement) -> bool
{
    return CanSelectTarget(hasTarget, hasEnhancement);
}
} // namespace automatoncontrollerenhancementselftarget
