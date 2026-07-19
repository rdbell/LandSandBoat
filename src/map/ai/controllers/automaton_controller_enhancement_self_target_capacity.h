#pragma once

namespace automatoncontrollerenhancementselftarget
{
inline auto CanSelectSelf(bool hasTarget, bool hasEnhancement) -> bool
{
    return !hasTarget && !hasEnhancement;
}
} // namespace automatoncontrollerenhancementselftarget
