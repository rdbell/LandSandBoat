#pragma once

#include "data/enums/ecosystem.h"
#include "modifier.h"

#include <cstdint>
#include <optional>

// Pure IsIntimidated halves after entity mod/status injects.
// Parity: internal/ecosystem intimidate.go

namespace intimidatehelpers
{

// IntimidateKillerMod is the inverse of addEcosystemKillerEffects' correlation:
// an attacker of ecosystem Eco is intimidated by defender gear of that same
// killer mod (Amorph attackers stopped by AMORPH_KILLER). Empty/Humanoid are
// mapped here even though they are unmapped on the killer-effect grant path.
// Luminian/Luminion are NOT swapped (unlike KillerMod).
inline auto IntimidateKillerMod(const xi::Ecosystem attackerEco) -> std::optional<Mod>
{
    switch (attackerEco)
    {
        case xi::Ecosystem::Amorph:
            return Mod::AMORPH_KILLER;
        case xi::Ecosystem::Aquan:
            return Mod::AQUAN_KILLER;
        case xi::Ecosystem::Arcana:
            return Mod::ARCANA_KILLER;
        case xi::Ecosystem::Beast:
            return Mod::BEAST_KILLER;
        case xi::Ecosystem::Bird:
            return Mod::BIRD_KILLER;
        case xi::Ecosystem::Demon:
            return Mod::DEMON_KILLER;
        case xi::Ecosystem::Dragon:
            return Mod::DRAGON_KILLER;
        case xi::Ecosystem::Empty:
            return Mod::EMPTY_KILLER;
        case xi::Ecosystem::Humanoid:
            return Mod::HUMANOID_KILLER;
        case xi::Ecosystem::Lizard:
            return Mod::LIZARD_KILLER;
        case xi::Ecosystem::Luminion:
            return Mod::LUMINION_KILLER;
        case xi::Ecosystem::Luminian:
            return Mod::LUMINIAN_KILLER;
        case xi::Ecosystem::Plantoid:
            return Mod::PLANTOID_KILLER;
        case xi::Ecosystem::Undead:
            return Mod::UNDEAD_KILLER;
        case xi::Ecosystem::Vermin:
            return Mod::VERMIN_KILLER;
        default:
            return std::nullopt;
    }
}

// IntimidateChance assembles the d100 threshold once entity identity and power
// injects are resolved. sameEntity short-circuits to 0 (cannot intimidate self).
// Negative intermediate sums pass through unchanged.
constexpr auto IntimidateChance(const bool         sameEntity,
                                const std::int16_t killerEffect,
                                const std::int16_t doubtPower,
                                const std::int16_t intimidatePower) -> std::int16_t
{
    if (sameEntity)
    {
        return 0;
    }
    return static_cast<std::int16_t>(killerEffect + doubtPower + intimidatePower);
}

// IsIntimidated reports whether roll (GetRandomNumber(100) → 0..99) is below
// chance. chance ≤ 0 never succeeds for a valid roll; chance ≥ 100 always does.
constexpr auto IsIntimidated(const std::int16_t chance, const std::uint8_t roll) -> bool
{
    return static_cast<std::int16_t>(roll) < chance;
}

} // namespace intimidatehelpers
