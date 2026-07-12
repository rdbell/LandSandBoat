#pragma once

#include <cstdint>

// Pure CanAffordSpell decision after entity/status/cost injects.

namespace canaffordspellhelpers
{

constexpr std::uint8_t MagicFlagsIgnoreMP = 1;

constexpr auto CanAffordSpell(const bool         hasManafont,
                              const std::uint8_t flags,
                              const bool         isMob,
                              const std::int16_t noSpellCost,
                              const bool         hasMPCost,
                              const std::uint16_t spellCost,
                              const std::int16_t currentMP) -> bool
{
    if (hasManafont || (flags & MagicFlagsIgnoreMP) != 0)
    {
        return true;
    }
    if (isMob && noSpellCost > 0)
    {
        return true;
    }
    if (hasMPCost)
    {
        return static_cast<std::int32_t>(currentMP) >= static_cast<std::int32_t>(spellCost);
    }
    return true;
}

} // namespace canaffordspellhelpers
