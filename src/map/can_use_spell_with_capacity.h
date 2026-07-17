#pragma once

#include <cstdint>

// Pure spell::CanUseSpellWith after GetSpell + getJob injects.
// Parity: internal/spell.CanUseSpellWith
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1731: CanUseSpellWith residual pure port
//   - 2970: CanUseSpellWith residual dual-wire suite
//           (!hasSpell → false; else level > jobLevel)
//   - 3159: CanUseSpellWith dedicated dual-wire (can_use_spell_with.go)
//           (hasSpell && level > jobLevel; STRICT >; formula unchanged)
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
// Formula (slice 3159 dedicated dual-wire; residual expand 2970 / pure 1731 —
// formula unchanged):
//   if !hasSpell → false
//   else         → level > jobLevel   // STRICT greater-than, not >=
//
// Equivalent one-liner:
//   hasSpell && level > jobLevel
//
// Positive if/else pin (production + 3159 tests; avoid QF1001 De Morgan):
//   if (hasSpell) { return level > jobLevel; }
//   return false;
//
// hasSpell — host-evaluated GetSpell(spellId) != nullptr
// jobLevel — host-evaluated getJob(job) after 0→255 mapping
// level    — host-evaluated caster / mob level
// true  → mobutils may assign the spell to the odd-job monster
// false → do not assign
//
// Dual-wire of Go spell.CanUseSpellWith (slice 3159 dedicated; residual 2970).
// Prior pure port: slice 1731. Residual dual-wire suite: 2970 /
// test_spell_can_use_with_2970. Dedicated dual-wire suite is
// test_spell_can_use_spell_with_3159. Host still owns GetSpell / getJob /
// mobutils assignment.
// Host inject (spell.cpp / mobutils):
//   if (CanUseSpellWith(GetSpell(id) != nullptr, getJob(...), level)) ...
//
// Matches Go spell.CanUseSpellWith (1731 residual / 2970 dual-wire / 3159 dedicated).
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
