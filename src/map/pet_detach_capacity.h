#pragma once

#include <cstdint>

// Pure DetachPet / DespawnPet gates from petutils.
// Parity: internal/petutils/detach.go (slice 1626 residual suite);
// internal/petutils/die_owned_pet.go (slice 3119 dual-wire ShouldDieOwnedPet);
// internal/petutils/clear_avatar_perpetuation.go (slice 3137 dual-wire
// ShouldClearAvatarPerpetuation).
//
// Dual-wire index:
//   - 3119: ShouldDieOwnedPet (!isDead on DetachPet TYPE_PET / OwnedPet branch)
//   - 3137: ShouldClearAvatarPerpetuation (petType == AVATAR on OwnedPet branch)
//
// Production host: petutils::DetachPet (petutils.cpp) injects master/pet/PC
// into ValidateDetachPet; classifies pet objtype via ClassifyDetachPet; on
// OwnedPet branch injects PPet->isDead() into ShouldDieOwnedPet; on true
// PPet->Die(); then injects getPetType() into ShouldClearAvatarPerpetuation;
// on true clears AVATAR_PERPETUATION. Go dual-wire:
// petutils.ShouldDieOwnedPet (die_owned_pet.go),
// petutils.ShouldClearAvatarPerpetuation (clear_avatar_perpetuation.go).
// Residual Validate*/Classify/PlanCharmedMobAlive remain 1626 residual.

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

// --- Slice 3119: ShouldDieOwnedPet pure dual-wire ---
//
// TYPE_PET / OwnedPet branch Die() gate on DetachPet after ClassifyDetachPet
// selects OwnedPet. Prior pure port: slice 1626 (DetachPet / DespawnPet pure
// gates). Go dual-wire: petutils.ShouldDieOwnedPet
// (internal/petutils/die_owned_pet.go).
// Sibling residual helpers left alone this slice: ValidateDetach*,
// ClassifyDetachPet, PlanCharmedMobAlive. Sibling dual-wire 3137:
// ShouldClearAvatarPerpetuation (clear gate after Die on OwnedPet).
//
// Formula (slice 3119 dual-wire):
//   !isDead
//
// isDead — host-evaluated PPet->isDead()
// true  → call PPet->Die() (owned pet still alive at detach)
// false → skip Die() (already dead)
//
// Dual-wire of Go petutils.ShouldDieOwnedPet.
// Call site: petutils::DetachPet — host injects PPet->isDead() on the
// OwnedPet branch; on true PPet->Die(). Residual pins remain in
// test_pet_detach_1626; dedicated dual-wire suite is
// test_pet_die_owned_3119.
inline auto ShouldDieOwnedPet(const bool isDead) -> bool
{
    return !isDead;
}

// --- Slice 3137: ShouldClearAvatarPerpetuation pure dual-wire ---
//
// TYPE_PET / OwnedPet branch AVATAR_PERPETUATION clear gate on DetachPet after
// ClassifyDetachPet selects OwnedPet and after ShouldDieOwnedPet Die gate.
// Prior pure port: slice 1626 (DetachPet / DespawnPet pure gates). Go
// dual-wire: petutils.ShouldClearAvatarPerpetuation
// (internal/petutils/clear_avatar_perpetuation.go).
// Sibling residual helpers left alone this slice: ValidateDetach*,
// ClassifyDetachPet, PlanCharmedMobAlive. Sibling dual-wire 3119:
// ShouldDieOwnedPet (Die gate before avatar clear on OwnedPet).
//
// Formula (slice 3137 dual-wire):
//   petType == PetTypeAvatar
//
// petType — host-evaluated PPetEnt->getPetType() (PET_TYPE)
// true  → clear master's Mod::AVATAR_PERPETUATION (set to 0)
// false → leave perpetuation mod alone
//
// Dual-wire of Go petutils.ShouldClearAvatarPerpetuation.
// Call site: petutils::DetachPet — host injects getPetType() on the OwnedPet
// branch; on true PMaster->setModifier(Mod::AVATAR_PERPETUATION, 0). Residual
// pins remain in test_pet_detach_1626; dedicated dual-wire suite is
// test_pet_clear_avatar_3137.
inline auto ShouldClearAvatarPerpetuation(const std::uint8_t petType) -> bool
{
    return petType == PetTypeAvatar;
}

} // namespace petdetachhelpers
