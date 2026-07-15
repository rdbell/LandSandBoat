#pragma once

#include <cstdint>

// Pure isUsingH2H / IsUsingH2H with fully injected inputs.
// Parity: internal/usingh2h (slice 1666).
//
// References:
//   - src/map/lua/lua_base_entity.cpp CLuaBaseEntity::isUsingH2H (~13196–13227)
//   - src/map/utils/battleutils.cpp IsUsingH2H (~1772–1784),
//     BattleEntityIsUsingH2H (~2041–2063)
//
// Host retains objtype, SLOT_MAIN equip / m_Weapons, and skill-type resolution;
// helpers take injected presence and skill values only.

namespace usingh2hhelpers
{

// SkillIndexH2H mirrors SKILLTYPE::SKILL_HAND_TO_HAND.
// Callers may pass this as h2hSkillType or any injected H2H skill id for tests.
inline constexpr std::uint8_t SkillIndexH2H = 1;

// IsPCUsingH2H mirrors the PC branch:
//   if !mainPresent: return true  // bare handed
//   return mainSkillType == h2hSkillType
inline auto IsPCUsingH2H(const bool         mainPresent,
                         const std::uint8_t mainSkillType,
                         const std::uint8_t h2hSkillType) -> bool
{
    if (!mainPresent)
    {
        return true;
    }
    return mainSkillType == h2hSkillType;
}

// IsMobPetUsingH2H mirrors the mob/pet branch:
//   return mainPresent && mainSkillType == h2hSkillType
// Empty main is never H2H for mobs/pets (unlike PCs).
inline auto IsMobPetUsingH2H(const bool         mainPresent,
                             const std::uint8_t mainSkillType,
                             const std::uint8_t h2hSkillType) -> bool
{
    return mainPresent && mainSkillType == h2hSkillType;
}

// IsUsingH2H resolves entity kind then applies the matching branch.
//   if isPC: return IsPCUsingH2H(...)
//   if isMobOrPet: return IsMobPetUsingH2H(...)
//   return false
// isPC wins if both kind flags are set. NPC / other kinds return false.
inline auto IsUsingH2H(const bool         isPC,
                       const bool         isMobOrPet,
                       const bool         mainPresent,
                       const std::uint8_t mainSkillType,
                       const std::uint8_t h2hSkillType) -> bool
{
    if (isPC)
    {
        return IsPCUsingH2H(mainPresent, mainSkillType, h2hSkillType);
    }
    if (isMobOrPet)
    {
        return IsMobPetUsingH2H(mainPresent, mainSkillType, h2hSkillType);
    }
    return false;
}

} // namespace usingh2hhelpers
