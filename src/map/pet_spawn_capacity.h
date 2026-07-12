#pragma once

#include <cstdint>
#include <vector>

// Pure CPetEntity::Spawn transition (elemental init + jug timer) and
// SetupPetWithMaster / SpawnPet / LoadPet gates from petutils.
// Parity: internal/petutils (entity spawn + setup_spawn; slices 8b696d / 1625).

namespace petspawnhelpers
{

// --- CPetEntity::Spawn (existing) ---

inline auto ShouldInitializeElemental(
    const bool hasMaster,
    const bool masterIsPlayer,
    const bool elementalEcosystem) -> bool
{
    return hasMaster && masterIsPlayer && elementalEcosystem;
}

template <typename SetMagicDelay, typename SetMagicCool, typename LoadSpells,
          typename SetJugSpawnTime, typename SpawnBase, typename EmitLuaSpawn>
inline void Apply(
    const bool initializeElemental,
    const bool jugPet,
    SetMagicDelay&& setMagicDelay,
    SetMagicCool&& setMagicCool,
    LoadSpells&& loadSpells,
    SetJugSpawnTime&& setJugSpawnTime,
    SpawnBase&& spawnBase,
    EmitLuaSpawn&& emitLuaSpawn)
{
    if (initializeElemental)
    {
        setMagicDelay();
        setMagicCool();
        loadSpells();
    }
    if (jugPet)
    {
        setJugSpawnTime();
    }
    spawnBase();
    emitLuaSpawn();
}

// --- SetupPetWithMaster / SpawnPet / LoadPet pure gates (slice 1625) ---

constexpr std::uint32_t MaxPetID = 128;

// PET_TYPE::AUTOMATON
constexpr std::uint8_t PetTypeAutomaton = 4;

// Status effect pins (effect.codegen.lua).
constexpr std::uint16_t StatusImpairment   = 261;
constexpr std::uint16_t StatusOmerta       = 262;
constexpr std::uint16_t StatusDebilitation = 263;

inline auto ShouldAddPetTraits(const std::uint8_t petType) -> bool
{
    return petType != PetTypeAutomaton;
}

inline auto ShouldSetupPetWithMaster(const bool masterIsPC) -> bool
{
    return masterIsPC;
}

inline auto SpawnPetBlocked(const bool hasPet) -> bool
{
    return hasPet;
}

inline auto LoadPetIDValid(const std::uint32_t petID) -> bool
{
    return petID < MaxPetID;
}

inline auto UseNormalSpawnAnimation(const bool spawningFromZone) -> bool
{
    return spawningFromZone;
}

inline auto ShouldLoadZoningInfo(const bool spawningFromZone) -> bool
{
    return spawningFromZone;
}

// Ordered effect IDs for active master statuses (Debilitation, Omerta, Impairment).
inline auto ActivePropagatedStatuses(const bool hasDebilitation, const bool hasOmerta, const bool hasImpairment) -> std::vector<std::uint16_t>
{
    std::vector<std::uint16_t> out;
    if (hasDebilitation)
    {
        out.push_back(StatusDebilitation);
    }
    if (hasOmerta)
    {
        out.push_back(StatusOmerta);
    }
    if (hasImpairment)
    {
        out.push_back(StatusImpairment);
    }
    return out;
}

} // namespace petspawnhelpers
