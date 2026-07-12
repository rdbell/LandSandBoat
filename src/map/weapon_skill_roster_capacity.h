#pragma once

#include <cstdint>

// Pure weapon-skill roster policy from charutils:
// CheckWeaponSkill, BuildingCharWeaponSkills, CheckUnarmedWeapon.

namespace weaponskillrosterhelpers
{

// SKILLTYPE pins from battle_entity.h.
constexpr std::uint8_t SkillHandToHand = 1;
constexpr std::uint8_t SkillThrowing   = 27;

// H2H look model pin used for unarmed H2H animations.
constexpr std::uint16_t UnarmedH2HLookMain = 21;

// IsMatchingWeaponSkill mirrors weapon present and getSkillType() == skill.
constexpr auto IsMatchingWeaponSkill(const bool hasWeapon, const std::uint8_t weaponSkillType, const std::uint8_t skill) -> bool
{
    return hasWeapon && weaponSkillType == skill;
}

// ShouldSkipCheckWeaponSkill mirrors no valid main AND no valid ranged for skill.
constexpr auto ShouldSkipCheckWeaponSkill(const bool mainMatches, const bool rangedMatches) -> bool
{
    return !mainMatches && !rangedMatches;
}

// RealSkillLevels is RealSkills.skill / 10 (shared with buildingskills).
constexpr auto RealSkillLevels(const std::uint16_t realSkillTenths) -> std::uint16_t
{
    return static_cast<std::uint16_t>(realSkillTenths / 10);
}

// ShouldUnlockWeaponSkillOnSkillUp mirrors curSkill == wsSkillLevel && canUse.
constexpr auto ShouldUnlockWeaponSkillOnSkillUp(const std::uint16_t curSkillLevels, const std::uint16_t wsSkillLevel, const bool canUse) -> bool
{
    return curSkillLevels == wsSkillLevel && canUse;
}

// ShouldUseUnlockableWeaponMod mirrors weapon present && (!unlockable || unlocked).
constexpr auto ShouldUseUnlockableWeaponMod(const bool hasWeapon, const bool isUnlockable, const bool isUnlocked) -> bool
{
    return hasWeapon && (!isUnlockable || isUnlocked);
}

// MeleeSkillTypeFromMain mirrors PItem ? skillType : SKILL_HAND_TO_HAND.
constexpr auto MeleeSkillTypeFromMain(const bool hasMainWeapon, const std::uint8_t mainSkillType) -> std::uint8_t
{
    return hasMainWeapon ? mainSkillType : SkillHandToHand;
}

// ShouldAddMeleeWeaponSkill mirrors canUse || wsID == main_ws unlock mod.
constexpr auto ShouldAddMeleeWeaponSkill(const bool canUse, const std::uint16_t wsID, const std::int32_t mainAddsWS) -> bool
{
    return canUse || static_cast<std::int32_t>(wsID) == mainAddsWS;
}

// ShouldConsiderRangedWeaponSkills mirrors ranged present, is weapon, skill != THROWING.
constexpr auto ShouldConsiderRangedWeaponSkills(const bool hasRanged, const bool isWeapon, const std::uint8_t rangedSkillType) -> bool
{
    return hasRanged && isWeapon && rangedSkillType != SkillThrowing;
}

// RangedSkillTypeFromItem mirrors PItem ? skillType : 0 (only called when item non-null in host).
constexpr auto RangedSkillTypeFromItem(const bool hasRanged, const std::uint8_t rangedSkillType) -> std::uint8_t
{
    return hasRanged ? rangedSkillType : 0;
}

// ShouldAddRangedWeaponSkill mirrors canUse || wsID == range_ws unlock mod.
constexpr auto ShouldAddRangedWeaponSkill(const bool canUse, const std::uint16_t wsID, const std::int32_t rangeAddsWS) -> bool
{
    return canUse || static_cast<std::int32_t>(wsID) == rangeAddsWS;
}

// ShouldUseUnarmedH2H mirrors (main or sub H2H rank > 0) && sub empty/non-equipment.
constexpr auto ShouldUseUnarmedH2H(const bool mainH2HRankPositive, const bool subH2HRankPositive, const bool subEmptyOrNonEquipment) -> bool
{
    return (mainH2HRankPositive || subH2HRankPositive) && subEmptyOrNonEquipment;
}

// IsSubEmptyOrNonEquipment mirrors !sub || !isType(ITEM_EQUIPMENT).
constexpr auto IsSubEmptyOrNonEquipment(const bool hasSub, const bool subIsEquipment) -> bool
{
    return !hasSub || !subIsEquipment;
}

// UnarmedLookMain returns 21 for H2H unarmed, else 0.
constexpr auto UnarmedLookMain(const bool useUnarmedH2H) -> std::uint16_t
{
    return useUnarmedH2H ? UnarmedH2HLookMain : 0;
}

} // namespace weaponskillrosterhelpers
