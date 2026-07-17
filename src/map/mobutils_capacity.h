#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure mobutils dual-wire helpers (OmegaXI internal/mobutils).
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2653: residual pure port (entities/mob_gil_policy.h mobgilhelpers)
//   - 2960: CanDropGil (gilMin/gilMax/gilBonus eligibility gate)
//   - 2972: ShouldAssignParrySkill (MOBMOD_CAN_PARRY > 0 gate; prior dual-wire)
//   - 3022: ShouldAssignGuardSkill (MNK/PUP + MOBMOD_CANNOT_GUARD == 0 gate)
//   - 3115: ShouldAssignParrySkill (MOBMOD_CAN_PARRY > 0 gate; re-index dual-wire)
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

// CanDropGil mirrors CMobEntity::CanDropGil pure half (slice 2960):
//
//   gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
//
// A negative gil maximum is a suppression sentinel and takes precedence
// over minimum and bonus modifiers. Matches Go mobutils.CanDropGil and
// residual mobgilhelpers::CanDropGil (2653).
inline auto CanDropGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0);
}

// CanStealGil mirrors CMobEntity::CanStealGil, which delegates to
// CanDropGil (same free-function dual-wire; slice 2960).
inline auto CanStealGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return CanDropGil(gilMin, gilMax, gilBonus);
}

// Slice 3115 — CalculateMobStats parry skill assignment gate
// (prior dual-wire expansion: slice 2972; residual pure port: 1623)
//
// Dual-wire notes (slice 3115):
//   Formula unchanged from residual 1623 / prior 2972 dual-wire:
//     ShouldAssignParrySkill(canParryMod) = canParryMod > 0
//   Go dual-wire: mobutils.ShouldAssignParrySkill
//   Index 3115: mobutils.ShouldAssignParrySkill pure dual-wire.
//   Prior dual-wire suite: test_mobutils_assign_parry_2972.
//   Dedicated dual-wire suite: test_mobutils_assign_parry_3115.
//
// ShouldAssignParrySkill mirrors CalculateMobStats parry skill assignment
// pure half (slice 3115 dual-wire; unchanged):
//
//   canParryMod > 0
//
// canParryMod is host-evaluated getMobMod(MOBMOD_CAN_PARRY). When true the
// host assigns WorkingSkills.skill[SKILL_PARRY] from GetBaseSkill using the
// mod value as rank. Matches Go mobutils.ShouldAssignParrySkill and residual
// mobsetuphelpers::ShouldAssignParrySkill (1623 / mob_setup_capacity.h).
// Dual-wire of Go mobutils.ShouldAssignParrySkill (assign_parry.go / slice 3115).
inline auto ShouldAssignParrySkill(const int16 canParryMod) -> bool
{
    return canParryMod > 0;
}

// ShouldAssignGuardSkill mirrors CalculateMobStats guard skill assignment
// pure half (slice 3022):
//
//   (mJob == 2 /*JOB_MNK*/ || mJob == 18 /*JOB_PUP*/) && cannotGuardMod == 0
//
// mJob is host-evaluated GetMJob(); cannotGuardMod is host-evaluated
// getMobMod(MOBMOD_CANNOT_GUARD). When true the host assigns
// WorkingSkills.skill[SKILL_GUARD] from GetBaseSkill using GuardSkillRank
// (fixed rank C / 3). Matches Go mobutils.ShouldAssignGuardSkill and residual
// mobsetuphelpers::ShouldAssignGuardSkill (1623 / mob_setup_capacity.h).
inline auto ShouldAssignGuardSkill(const uint8 mJob, const int16 cannotGuardMod) -> bool
{
    return (mJob == 2 /*JOB_MNK*/ || mJob == 18 /*JOB_PUP*/) && cannotGuardMod == 0;
}

// GuardSkillRank is the fixed rank C used for MNK/PUP guard assignment when
// ShouldAssignGuardSkill is true. Matches Go mobutils.GuardSkillRank and
// residual mobsetuphelpers::GuardSkillRank (1623 / mob_setup_capacity.h).
constexpr uint8 GuardSkillRank = 3;

} // namespace mobutilshelpers
