#pragma once

#include <cstdint>

// Pure spell::CanUseSpellWith after GetSpell + getJob injects.
// Parity: internal/spell.CanUseSpellWith
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1731: CanUseSpellWith residual pure port
//   - 2970: CanUseSpellWith dual-wire (!hasSpell → false; else level > jobLevel)
//
// Production host: spell::CanUseSpellWith / mobutils injects
// GetSpell(spellId) != nullptr and getJob(job) into CanUseSpellWith.
// Go dual-wire: spell.CanUseSpellWith
// (internal/spell/can_use_spell_with.go).
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
// Distinct from CanUseSpell job gates (strict >=; can_use_spell_job_gates).

namespace canusespellwithhelpers
{

// CanUseSpellWith is the pure form once catalog presence and job level are
// injected. hasSpell mirrors GetSpell(spellId) != nullptr. jobLevel is the
// value returned by getJob (0→255 mapping already applied by the host).
//
// Formula (slice 2970 dual-wire):
//   if !hasSpell → false
//   else         → level > jobLevel   // STRICT greater-than, not >=
//
// Equivalent one-liner:
//   hasSpell && level > jobLevel
//
// hasSpell — host-evaluated GetSpell(spellId) != nullptr
// jobLevel — host-evaluated getJob(job) after 0→255 mapping
// level    — host-evaluated caster / mob level
// true  → mobutils may assign the spell to the odd-job monster
// false → do not assign
//
// Dual-wire of Go spell.CanUseSpellWith.
// Host inject (spell.cpp / mobutils):
//   if (CanUseSpellWith(GetSpell(id) != nullptr, getJob(...), level)) ...
//
// Matches Go spell.CanUseSpellWith (1731 residual / 2970 dual-wire).
// (*Spell).CanUseWithJob dual-wires through the same free function.
constexpr auto CanUseSpellWith(const bool hasSpell, const std::uint8_t jobLevel, const std::uint8_t level) -> bool
{
    if (!hasSpell)
    {
        return false;
    }
    return level > jobLevel;
}

} // namespace canusespellwithhelpers
