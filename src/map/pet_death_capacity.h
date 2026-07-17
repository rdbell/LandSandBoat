#pragma once

// Pure CPetEntity::Die death-transition gates and orchestration extracted so
// native tests can pin policy without entity/AI/Lua instances.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2951: ShouldDespawnForZoning residual dual-wire suite
//   - 3170: ShouldDespawnForZoning dedicated dual-wire (despawn_zoning.go)
//   - 2987: ShouldDetachPlayerMaster (player-master active-pet detach gate)
//   - residual 1414: Apply orchestration
//
// Dual-wire index:
//   - 2951: ShouldDespawnForZoning residual dual-wire suite
//   - 3170: ShouldDespawnForZoning =
//           hpPositive && hasMaster && masterIsPlayer && respawnPet
//   - 2987: ShouldDetachPlayerMaster
//
// Production host: CPetEntity::Die (entities/pet_entity.cpp) injects health /
// master / petZoningInfo scalars into ShouldDespawnForZoning and
// ShouldDetachPlayerMaster before petdeathhelpers::Apply.
// Go dual-wire: petentity.ShouldDespawnForZoning
// (internal/petentity/despawn_zoning.go), petentity.ShouldDetachPlayerMaster
// (internal/petentity/detach_player_master.go).
// Residual dual-wire suite: 2951 (test_petentity_despawn_zoning_2951).
// Dedicated dual-wire suite: 3170 (test_petentity_despawn_zoning_3170).
// Prior pure port: slices 1414 / 2261 / 2262.

namespace petdeathhelpers
{

// ---------------------------------------------------------------------------
// Slice 3170 — CPetEntity::Die zoning-despawn gate
// (dedicated expand residual 2951)
// ---------------------------------------------------------------------------

// ShouldDespawnForZoning reports whether CPetEntity::Die should force-despawn
// (preserve for zoning respawn) instead of entering the death state.
//
// Formula (slice 3170 dedicated dual-wire; residual expand 2951 / pure 1414 /
// 2261 — formula unchanged):
//   hpPositive && hasMaster && masterIsPlayer && respawnPet
//
// Host-injected scalars (no entity pointers):
//   hpPositive     — health.hp > 0
//   hasMaster      — PMaster != nullptr
//   masterIsPlayer — PMaster != nullptr && PMaster->objtype == TYPE_PC
//   respawnPet     — masterIsPlayer &&
//                    static_cast<CCharEntity*>(PMaster)->petZoningInfo.respawnPet
// true  → host takes Internal_Despawn branch so the pet can be restored
// false → host enters Internal_Die death state
//
// Dual-wire of Go petentity.ShouldDespawnForZoning
// (internal/petentity/despawn_zoning.go).
// Call site: CPetEntity::Die (pet_entity.cpp).
// Prior pure port: slices 1414 / 2261.
// Residual dual-wire suite: 2951 / test_petentity_despawn_zoning_2951.
// Dedicated dual-wire suite: 3170 / test_petentity_despawn_zoning_3170.
// Sibling residual only under 3170 (not re-expanded):
// ShouldDetachPlayerMaster (2987 dual-wire).
// Coverage: test_petentity_despawn_zoning_3170 (not in CMake/main).
inline auto ShouldDespawnForZoning(
    const bool hpPositive,
    const bool hasMaster,
    const bool masterIsPlayer,
    const bool respawnPet) -> bool
{
    return hpPositive && hasMaster && masterIsPlayer && respawnPet;
}

// ShouldDetachPlayerMaster reports whether the dying pet still occupies its
// player master's active pet slot so Die can call DetachPet.
//
// Formula (slice 2987 dual-wire):
//   hasMaster && masterPetIsSelf && masterIsPlayer
//
// Host-injected scalars (no entity pointers):
//   hasMaster       — PMaster != nullptr
//   masterPetIsSelf — PMaster != nullptr && PMaster->PPet == this
//   masterIsPlayer  — PMaster != nullptr && PMaster->objtype == TYPE_PC
// true  → host calls petutils::DetachPet(PMaster) after base Die
// false → host leaves master's PPet alone
//
// Dual-wire of Go petentity.ShouldDetachPlayerMaster
// (internal/petentity/detach_player_master.go).
// Prior pure port: slices 1414 / 2262.
// Call site: CPetEntity::Die (pet_entity.cpp).
// Sibling zoning-despawn dual-wire: ShouldDespawnForZoning (3170 dedicated;
// residual 2951).
inline auto ShouldDetachPlayerMaster(
    const bool hasMaster,
    const bool masterPetIsSelf,
    const bool masterIsPlayer) -> bool
{
    return hasMaster && masterPetIsSelf && masterIsPlayer;
}

template <typename ClearStates, typename Despawn, typename EnterDeath,
          typename EmitLuaDeath, typename FinalizeBase, typename DetachMaster>
inline void Apply(
    const bool despawnForZoning,
    const bool detachPlayerMaster,
    ClearStates&& clearStates,
    Despawn&& despawn,
    EnterDeath&& enterDeath,
    EmitLuaDeath&& emitLuaDeath,
    FinalizeBase&& finalizeBase,
    DetachMaster&& detachMaster)
{
    clearStates();
    if (despawnForZoning)
    {
        despawn();
    }
    else
    {
        enterDeath();
    }
    emitLuaDeath();
    finalizeBase();
    if (detachPlayerMaster)
    {
        detachMaster();
    }
}

} // namespace petdeathhelpers
