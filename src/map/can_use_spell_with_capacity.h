#pragma once

#include <cstdint>

// Pure spell::CanUseSpellWith after GetSpell + getJob injects.
// Parity: internal/spell.CanUseSpellWith (slice 1731).
//
// Reference:
//   src/map/spell.cpp CanUseSpellWith (~842–853)
//
//   if (GetSpell(spellId) != nullptr)
//   {
//       uint8 jobMLevel = PSpellList[spellId]->getJob(job);
//       return level > jobMLevel;  // STRICT greater-than, not >=
//   }
//   return false;
//
// getJob maps stored CANNOT_USE_SPELL (0) to 255, so jobLevel 255 is always
// unusable (level > 255 is impossible for uint8).

namespace canusespellwithhelpers
{

// CanUseSpellWith is the pure form once catalog presence and job level are
// injected. hasSpell mirrors GetSpell(spellId) != nullptr. jobLevel is the
// value returned by getJob (0→255 mapping already applied by the host).
constexpr auto CanUseSpellWith(const bool hasSpell, const std::uint8_t jobLevel, const std::uint8_t level) -> bool
{
    if (!hasSpell)
    {
        return false;
    }
    return level > jobLevel;
}

} // namespace canusespellwithhelpers
