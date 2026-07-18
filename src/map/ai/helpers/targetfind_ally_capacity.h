#pragma once

namespace targetfindallyhelpers
{

// ShouldRejectSelfCenteredAllyOnly preserves validEntity's Benediction-style
// gate and lazily resolves allegiance only for self-centered ally-only AoEs.
template <typename MasterAllegianceMismatch>
inline auto ShouldRejectSelfCenteredAllyOnly(const bool selfCenteredAoE,
                                             const bool targetAnyAllegiance,
                                             const bool targetEnemy,
                                             MasterAllegianceMismatch&& masterAllegianceMismatch) -> bool
{
    if (!selfCenteredAoE || targetAnyAllegiance || targetEnemy)
    {
        return false;
    }
    return masterAllegianceMismatch();
}

} // namespace targetfindallyhelpers
