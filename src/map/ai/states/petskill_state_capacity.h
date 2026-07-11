#pragma once

#include "common/cbasetypes.h"

#include <cstdint>
#include <utility>

// Pure CPetSkillState policy halves (ctor status/skill-start gates, SpendCost,
// Update finish/enmity/Avatars Favor/avatar re-engage). Entity mutation,
// packets, and skill catalog remain host-injected.

namespace petskillstatehelpers
{

// Pet ID pins (petutils.h).
constexpr uint16 PetIDAlexander = 17;
constexpr uint16 PetIDAtomos    = 19;
constexpr uint16 PetIDWyvern    = 48;

// PET_TYPE::AVATAR pin (pet_entity.h).
constexpr uint8 PetTypeAvatar = 0;

// Avatars Favor pins (shared with mobskill).
constexpr int16 AvatarsFavorRageGain = 3;
constexpr int16 AvatarsFavorWardGain = 2;
constexpr int16 AvatarsFavorPowerMin = 11;

// MsgBasic pins for skill-start message selection.
constexpr uint16 MsgReadiesWeaponskill = 43;  // MsgBasic::ReadiesWeaponskill
constexpr uint16 MsgReadiesSkill       = 326; // MsgBasic::ReadiesSkill

// ShouldRejectAmnesiaOrImpairment mirrors ctor Amnesia/Impairment gate.
inline auto ShouldRejectAmnesiaOrImpairment(const bool hasAmnesiaOrImpairment) -> bool
{
    return hasAmnesiaOrImpairment;
}

// SkillStartParam mirrors mobSkillID > 0 ? mobSkillID : skillID.
inline auto SkillStartParam(const uint16 mobSkillID, const uint16 skillID) -> uint16
{
    return mobSkillID > 0 ? mobSkillID : skillID;
}

// SkillStartUsesWeaponskillMessage mirrors mobSkillID > 0 → ReadiesWeaponskill else ReadiesSkill.
inline auto SkillStartUsesWeaponskillMessage(const uint16 mobSkillID) -> bool
{
    return mobSkillID > 0;
}

// ShouldEmitWyvernSkillReady mirrors:
//   petID == WYVERN && WYVERN_SHOW_READYING == 0 (only when castTime > 0, host-gated).
inline auto ShouldEmitWyvernSkillReady(const uint16 petID, const int16 wyvernShowReadingMod) -> bool
{
    return petID == PetIDWyvern && wyvernShowReadingMod == 0;
}

// ShouldSpendPetSkillTP mirrors !isTpFreeSkill.
inline auto ShouldSpendPetSkillTP(const bool isTpFreeSkill) -> bool
{
    return !isTpFreeSkill;
}

// EvaluatePetSkillSpendCost: non-free spends all current TP (remaining 0).
// Free: spent 0, remaining = current.
inline auto EvaluatePetSkillSpendCost(const bool isTpFreeSkill, const int16 currentTP) -> std::pair<int16, int16>
{
    if (isTpFreeSkill)
    {
        return { 0, currentTP };
    }
    return { currentTP, 0 };
}

// ShouldFinishPetSkill mirrors tick > entry+cast && !completed (strict >).
inline auto ShouldFinishPetSkill(const bool tickAfterCastEnd, const bool completed) -> bool
{
    return tickAfterCastEnd && !completed;
}

// SkillSuccessFromAction mirrors !action.targets.empty().
inline auto SkillSuccessFromAction(const bool actionTargetsEmpty) -> bool
{
    return !actionTargetsEmpty;
}

// ShouldExitPetSkill mirrors IsCompleted() && tick > finishTime.
inline auto ShouldExitPetSkill(const bool completed, const bool tickAfterFinish) -> bool
{
    return completed && tickAfterFinish;
}

// ShouldUpdateExitEnmity mirrors:
//   skillSuccess && target && TYPE_MOB && target != self && allegiance != target.allegiance
inline auto ShouldUpdateExitEnmity(
    const bool skillSuccess,
    const bool hasTarget,
    const bool targetIsMob,
    const bool targetIsSelf,
    const bool differentAllegiance) -> bool
{
    return skillSuccess && hasTarget && targetIsMob && !targetIsSelf && differentAllegiance;
}

// EnmityWithMaster mirrors objtype == TYPE_PET (always true for CPetEntity).
inline auto EnmityWithMaster(const bool isPet) -> bool
{
    return isPet;
}

// ShouldApplyAvatarsFavor mirrors pet + PC master + BP + has favor.
inline auto ShouldApplyAvatarsFavor(
    const bool isPet,
    const bool masterIsPC,
    const bool isBloodPactRage,
    const bool isBloodPactWard,
    const bool hasAvatarsFavor) -> bool
{
    return isPet && masterIsPC && (isBloodPactRage || isBloodPactWard) && hasAvatarsFavor;
}

// AvatarsFavorLevelGained mirrors rage ? 3 : 2.
inline auto AvatarsFavorLevelGained(const bool isBloodPactRage) -> int16
{
    return isBloodPactRage ? AvatarsFavorRageGain : AvatarsFavorWardGain;
}

// ApplyAvatarsFavorPower mirrors power += gain; then power > 11 ? power : 11.
inline auto ApplyAvatarsFavorPower(const int16 currentPower, const int16 levelGained) -> int16
{
    const auto power = static_cast<int16>(currentPower + levelGained);
    return power > AvatarsFavorPowerMin ? power : AvatarsFavorPowerMin;
}

// ShouldConsiderAvatarReengage is the outer BP exit block for avatar re-engage:
//   isPet && master PC && (rage||ward) — target checks separate.
// (Called inside the same BP block as Avatars Favor in production.)
inline auto ShouldConsiderAvatarReengage(
    const bool isPet,
    const bool masterIsPC,
    const bool isBloodPactRage,
    const bool isBloodPactWard) -> bool
{
    return isPet && masterIsPC && (isBloodPactRage || isBloodPactWard);
}

// ShouldReengageAfterBloodPact mirrors avatar re-engage inner checks:
//   hasTarget && petType==AVATAR && petID not Alexander/Atomos &&
//   battle target alive TYPE_MOB different allegiance.
inline auto ShouldReengageAfterBloodPact(
    const bool hasTarget,
    const uint8 petType,
    const uint16 petID,
    const bool targetAlive,
    const bool targetIsMob,
    const bool differentAllegiance) -> bool
{
    if (!hasTarget)
    {
        return false;
    }
    if (petType != PetTypeAvatar)
    {
        return false;
    }
    if (petID == PetIDAlexander || petID == PetIDAtomos)
    {
        return false;
    }
    return targetAlive && targetIsMob && differentAllegiance;
}

// ShouldApplyFinalAnimationSub mirrors completed cleanup:
//   isAlive && finalAnimationSub has_value.
inline auto ShouldApplyFinalAnimationSub(const bool isAlive, const bool hasFinalAnimationSub) -> bool
{
    return isAlive && hasFinalAnimationSub;
}

// ShouldInterruptOnCleanup mirrors !IsCompleted() on Cleanup.
inline auto ShouldInterruptOnCleanup(const bool completed) -> bool
{
    return !completed;
}

// ShouldSendSkillStartPacket mirrors castTime > 0.
inline auto ShouldSendSkillStartPacket(const bool castTimePositive) -> bool
{
    return castTimePositive;
}

} // namespace petskillstatehelpers
