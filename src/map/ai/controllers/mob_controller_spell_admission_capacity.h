#pragma once

namespace mobcontrollerspelladmission
{
constexpr auto CanCastSpells(
    const bool hasSpells, const bool silenced, const bool muted, const bool isSummoner, const bool hasLivingPet,
    const bool magicCastingEnabled, const bool ignoreRecastsAndCosts, const bool anySpellAvailable) -> bool
{
    if (!hasSpells || silenced || muted || (isSummoner && hasLivingPet) || !magicCastingEnabled)
    {
        return false;
    }
    return ignoreRecastsAndCosts || anySpellAvailable;
}
} // namespace mobcontrollerspelladmission
