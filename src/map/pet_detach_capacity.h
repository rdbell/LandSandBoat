#pragma once

#include <cstdint>

// Pure DetachPet / DespawnPet gates from petutils.
// Parity: internal/petutils/detach.go (slice 1626).

namespace petdetachhelpers
{

// ENTITYTYPE pins (base_entity.h).
constexpr std::uint8_t EntityTypePC  = 0x01;
constexpr std::uint8_t EntityTypeMOB = 0x04;
constexpr std::uint8_t EntityTypePET = 0x08;

// PET_TYPE::AVATAR
constexpr std::uint8_t PetTypeAvatar = 0;

enum class DetachReject : std::uint8_t
{
    OK           = 0,
    MasterNull   = 1,
    PetNull      = 2,
    MasterNotPC  = 3,
};

inline auto ValidateDetachMaster(const bool hasMaster, const bool hasPet) -> DetachReject
{
    if (!hasMaster)
    {
        return DetachReject::MasterNull;
    }
    if (!hasPet)
    {
        return DetachReject::PetNull;
    }
    return DetachReject::OK;
}

inline auto ValidateDetachPet(const bool hasMaster, const bool hasPet, const bool masterIsPC) -> DetachReject
{
    const auto base = ValidateDetachMaster(hasMaster, hasPet);
    if (base != DetachReject::OK)
    {
        return base;
    }
    if (!masterIsPC)
    {
        return DetachReject::MasterNotPC;
    }
    return DetachReject::OK;
}

enum class DetachKind : std::uint8_t
{
    None        = 0,
    CharmedMob  = 1,
    OwnedPet    = 2,
};

inline auto ClassifyDetachPet(const std::uint8_t petObjType) -> DetachKind
{
    if (petObjType == EntityTypeMOB)
    {
        return DetachKind::CharmedMob;
    }
    if (petObjType == EntityTypePET)
    {
        return DetachKind::OwnedPet;
    }
    return DetachKind::None;
}

struct CharmedMobAlivePlan
{
    bool disengage              = true;
    bool withinEnmityRange      = false;
    bool giveExp                = false;
    bool clearEnmityLeaveOrDead = false;
};

inline auto PlanCharmedMobAlive(const bool withinEnmity, const bool leaveAbility, const bool masterDead, const bool hppFull) -> CharmedMobAlivePlan
{
    return CharmedMobAlivePlan{
        true,
        withinEnmity,
        hppFull,
        leaveAbility || masterDead,
    };
}

inline auto ShouldDieOwnedPet(const bool isDead) -> bool
{
    return !isDead;
}

inline auto ShouldClearAvatarPerpetuation(const std::uint8_t petType) -> bool
{
    return petType == PetTypeAvatar;
}

} // namespace petdetachhelpers
