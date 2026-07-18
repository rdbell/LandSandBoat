#pragma once

namespace targetfindfirsttargethelpers
{

// ShouldAcceptFirstTarget mirrors validEntity's post-filter shortcut.
inline auto ShouldAcceptFirstTarget(const bool hasInitialTarget, const bool selfCenteredAoE, const bool conal) -> bool
{
    return !hasInitialTarget && (!selfCenteredAoE || conal);
}

} // namespace targetfindfirsttargethelpers
