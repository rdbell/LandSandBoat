#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure mobutils dual-wire helpers (OmegaXI internal/mobutils).
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2653: residual pure port (entities/mob_gil_policy.h mobgilhelpers)
//   - 2960: CanDropGil (gilMin/gilMax/gilBonus eligibility gate; residual dual-wire)
//   - 2972: ShouldAssignParrySkill (MOBMOD_CAN_PARRY > 0 gate; residual dual-wire)
//   - 3022: ShouldAssignGuardSkill (MNK/PUP + MOBMOD_CANNOT_GUARD == 0 gate; prior dual-wire)
//   - 3115: ShouldAssignParrySkill (MOBMOD_CAN_PARRY > 0 gate; prior re-index dual-wire)
//   - 3131: ShouldAssignGuardSkill (MNK/PUP + MOBMOD_CANNOT_GUARD == 0 gate; re-index dual-wire)
//   - 3158: CanDropGil (gilMin/gilMax/gilBonus eligibility gate; prior dedicated dual-wire)
//   - 3231: CanDropGil (gilMin/gilMax/gilBonus eligibility gate; prior dedicated expand residual 2960)
//   - 3279: CanDropGil (gilMin/gilMax/gilBonus eligibility gate; prior dedicated expand residual 2960)
//   - 3309: CanDropGil (gilMin/gilMax/gilBonus eligibility gate; dedicated expand residual 2960)
//   - 3361: ShouldAssignParrySkill (MOBMOD_CAN_PARRY > 0 gate; dedicated expand residual 2972)
//
// Production hosts:
//   - CMobEntity::CanDropGil / CanStealGil in mob_entity.cpp injects
//     getMobMod(MOBMOD_GIL_MIN/MAX/BONUS) into mobgilhelpers::CanDropGil
//     (same formula; residual 2653 surface).
//   - CalculateMobStats in utils/mobutils.cpp injects
//     getMobMod(MOBMOD_CAN_PARRY) into mobsetuphelpers::ShouldAssignParrySkill
//     (same formula; residual 1623 surface in mob_setup_capacity.h).
//   - CalculateMobStats in utils/mobutils.cpp injects GetMJob +
//     getMobMod(MOBMOD_CANNOT_GUARD) into mobsetuphelpers::ShouldAssignGuardSkill
//     (same formula; residual 1623 surface in mob_setup_capacity.h).
//
// This capacity dual-wires free-function forms used by OmegaXI
// internal/mobutils so hosts call capacity helpers instead of re-inlining.
//
// CanDropGil:
//   gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
// CanStealGil dual-wires through the same free function (production
// CMobEntity::CanStealGil delegates to CanDropGil).
//
// ShouldAssignParrySkill:
//   canParryMod > 0
//
// ShouldAssignGuardSkill:
//   (mJob == 2 /*MNK*/ || mJob == 18 /*PUP*/) && cannotGuardMod == 0
// GuardSkillRank = 3 (fixed rank C when assign is true)
//
// Hosts inject scalars only (no CMobEntity*). Gil drop amount / steal
// amount / GetBaseSkill + WorkingSkills writeback remain host-owned.
//
// Go dual-wire:
//   - mobutils.CanDropGil / mobutils.CanStealGil (gil_policy.go)
//   - mobutils.ShouldAssignParrySkill (assign_parry.go)
//   - mobutils.ShouldAssignGuardSkill / mobutils.GuardSkillRank (assign_guard.go)

namespace mobutilshelpers
{

// Slice 3309 — CMobEntity gil eligibility gate
// (residual dual-wire expansion: slice 2960; prior dedicated expand residual
// 2960: 3279 / 3231; prior dedicated: 3158; residual pure port: 2653)
//
// Dual-wire notes (slice 3309 dedicated expand residual 2960):
//   Formula unchanged from residual 2653 / residual 2960 dual-wire /
//   prior dedicated 3158 / prior dedicated expand residual 2960 3231 / 3279:
//     CanDropGil(gilMin, gilMax, gilBonus) =
//       gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
//   Direct return form (production free function + 3309 / 3279 / 3231 inline/pin):
//     return gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
//   Positive if/else form (prior 3158 pin; avoid QF1001 De Morgan):
//     if gilMax < 0 → false
//     if gilMin > 0 → true
//     if gilMax != 0 → true
//     else → gilBonus > 0
//   Go dual-wire: mobutils.CanDropGil / mobutils.CanStealGil
//   Index 3309: mobutils.CanDropGil pure dual-wire (dedicated expand residual 2960).
//   Residual dual-wire suite: test_mobutils_can_drop_gil_2960.
//   Prior dedicated dual-wire suite: test_mobutils_can_drop_gil_3158.
//   Prior dedicated expand residual 2960 suites: test_mobutils_can_drop_gil_3231,
//     test_mobutils_can_drop_gil_3279.
//   Dedicated dual-wire suite: test_mobutils_can_drop_gil_3309.
//   Sibling parry dual-wire (3115) and guard dual-wire (3131) left alone.
//
// CanDropGil mirrors CMobEntity::CanDropGil pure half (slice 3309 dual-wire;
// unchanged):
//
//   gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
//
// A negative gil maximum is a suppression sentinel and takes precedence
// over minimum and bonus modifiers. Matches Go mobutils.CanDropGil and
// residual mobgilhelpers::CanDropGil (2653).
// Dual-wire of Go mobutils.CanDropGil (gil_policy.go / slice 3309).
inline auto CanDropGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0);
}

// CanStealGil mirrors CMobEntity::CanStealGil, which delegates to
// CanDropGil (same free-function dual-wire; slice 3309; residual 2960;
// prior dedicated expand 3279 / 3231; prior dedicated 3158).
inline auto CanStealGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return CanDropGil(gilMin, gilMax, gilBonus);
}

// Slice 3361 — CalculateMobStats parry skill assignment gate
// (dedicated expand residual 2972; prior re-index dual-wire: 3115;
// residual dual-wire expansion: 2972; residual pure port: 1623)
//
// Dual-wire notes (slice 3361 dedicated expand residual 2972):
//   Formula unchanged from residual 1623 / residual 2972 dual-wire /
//   prior re-index 3115:
//     ShouldAssignParrySkill(canParryMod) = canParryMod > 0
//   Direct return form (production free function + 3361 / 3115 / 2972 inline/pin):
//     return canParryMod > 0;
//   Go dual-wire: mobutils.ShouldAssignParrySkill
//   Index 3361: mobutils.ShouldAssignParrySkill pure dual-wire
//     (dedicated expand residual 2972).
//   Residual dual-wire suite: test_mobutils_assign_parry_2972.
//   Prior re-index dual-wire suite: test_mobutils_assign_parry_3115.
//   Dedicated dual-wire suite: test_mobutils_assign_parry_3361.
//   Sibling guard dual-wire (3131) left alone.
//
// ShouldAssignParrySkill mirrors CalculateMobStats parry skill assignment
// pure half (slice 3361 dual-wire; unchanged):
//
//   canParryMod > 0
//
// canParryMod is host-evaluated getMobMod(MOBMOD_CAN_PARRY). When true the
// host assigns WorkingSkills.skill[SKILL_PARRY] from GetBaseSkill using the
// mod value as rank. Matches Go mobutils.ShouldAssignParrySkill and residual
// mobsetuphelpers::ShouldAssignParrySkill (1623 / mob_setup_capacity.h).
// Dual-wire of Go mobutils.ShouldAssignParrySkill (assign_parry.go / slice 3361).
inline auto ShouldAssignParrySkill(const int16 canParryMod) -> bool
{
    return canParryMod > 0;
}

// Slice 3131 — CalculateMobStats guard skill assignment gate
// (prior dual-wire expansion: slice 3022; residual pure port: 1623)
//
// Dual-wire notes (slice 3131):
//   Formula unchanged from residual 1623 / prior 3022 dual-wire:
//     ShouldAssignGuardSkill(mJob, cannotGuardMod) =
//       (mJob == 2 /*MNK*/ || mJob == 18 /*PUP*/) && cannotGuardMod == 0
//   Go dual-wire: mobutils.ShouldAssignGuardSkill / GuardSkillRank
//   Index 3131: mobutils.ShouldAssignGuardSkill pure dual-wire.
//   Prior dual-wire suite: test_mobutils_assign_guard_3022.
//   Dedicated dual-wire suite: test_mobutils_assign_guard_3131.
//   Sibling parry dual-wire (3115) left alone.
//
// ShouldAssignGuardSkill mirrors CalculateMobStats guard skill assignment
// pure half (slice 3131 dual-wire; unchanged):
//
//   (mJob == 2 /*JOB_MNK*/ || mJob == 18 /*JOB_PUP*/) && cannotGuardMod == 0
//
// mJob is host-evaluated GetMJob(); cannotGuardMod is host-evaluated
// getMobMod(MOBMOD_CANNOT_GUARD). When true the host assigns
// WorkingSkills.skill[SKILL_GUARD] from GetBaseSkill using GuardSkillRank
// (fixed rank C / 3). Matches Go mobutils.ShouldAssignGuardSkill and residual
// mobsetuphelpers::ShouldAssignGuardSkill (1623 / mob_setup_capacity.h).
// Dual-wire of Go mobutils.ShouldAssignGuardSkill (assign_guard.go / slice 3131).
inline auto ShouldAssignGuardSkill(const uint8 mJob, const int16 cannotGuardMod) -> bool
{
    return (mJob == 2 /*JOB_MNK*/ || mJob == 18 /*JOB_PUP*/) && cannotGuardMod == 0;
}

// GuardSkillRank is the fixed rank C used for MNK/PUP guard assignment when
// ShouldAssignGuardSkill is true. Matches Go mobutils.GuardSkillRank and
// residual mobsetuphelpers::GuardSkillRank (1623 / mob_setup_capacity.h).
constexpr uint8 GuardSkillRank = 3;

} // namespace mobutilshelpers
