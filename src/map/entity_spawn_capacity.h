#pragma once

#include <cstdint>

// Pure hasEntitySpawned, raceChange bounds, and HomePoint admission from charutils.

namespace entityspawnhelpers
{

// ENTITYTYPE pins commonly used for spawn lists.
constexpr std::uint8_t TypePC    = 0x01;
constexpr std::uint8_t TypeNPC   = 0x02;
constexpr std::uint8_t TypeMob   = 0x04;
constexpr std::uint8_t TypePet   = 0x08;
constexpr std::uint8_t TypeTrust = 0x20; // verify against baseentity if tests fail

// Spawn list kind for pure mapping.
enum class SpawnListKind : std::uint8_t
{
    None = 0,
    Mob,
    NPC,
    PC,
    Pet,
    Trust,
};

// ShouldRejectNullEntity mirrors !entity.
constexpr auto ShouldRejectNullEntity(const bool hasEntity) -> bool
{
    return !hasEntity;
}

// SpawnListKindFromObjType maps objtype to which spawn list to consult.
// Host supplies TYPE_* values matching LSB ENTITYTYPE.
constexpr auto SpawnListKindFromObjType(const std::uint8_t objtype, const std::uint8_t typeMob, const std::uint8_t typeNPC, const std::uint8_t typePC, const std::uint8_t typePet, const std::uint8_t typeTrust) -> SpawnListKind
{
    if (objtype == typeMob)
    {
        return SpawnListKind::Mob;
    }
    if (objtype == typeNPC)
    {
        return SpawnListKind::NPC;
    }
    if (objtype == typePC)
    {
        return SpawnListKind::PC;
    }
    if (objtype == typePet)
    {
        return SpawnListKind::Pet;
    }
    if (objtype == typeTrust)
    {
        return SpawnListKind::Trust;
    }
    return SpawnListKind::None;
}

// IsSpawnedInList mirrors find(id) != end.
constexpr auto IsSpawnedInList(const bool foundInList) -> bool
{
    return foundInList;
}

// --- raceChange pure bounds ---

// CharRace HumeMale=1 .. Galka=8, CharFace Face8B, CharSize Large=2 (from char_entity.h).
constexpr std::uint8_t CharRaceHumeMale = 1;
constexpr std::uint8_t CharRaceGalka    = 8;
constexpr std::uint8_t CharFaceFace8B   = 15; // Face1A=0 ... Face8B = 15 typically
constexpr std::uint8_t CharSizeLarge    = 2;

// IsRaceChangeArgsInBounds mirrors race/face/size range checks.
constexpr auto IsRaceChangeArgsInBounds(const std::uint8_t newRace, const std::uint8_t newFace, const std::uint8_t newSize, const std::uint8_t raceMin, const std::uint8_t raceMax, const std::uint8_t faceMax, const std::uint8_t sizeMax) -> bool
{
    return newRace >= raceMin && newRace <= raceMax && newFace <= faceMax && newSize <= sizeMax;
}

// ShouldRejectNullCharRaceChange mirrors !PChar.
constexpr auto ShouldRejectNullCharRaceChange(const bool hasChar) -> bool
{
    return !hasChar;
}

// ShouldUnequipOnRaceChange mirrors !isEquippableByRace(newRace).
constexpr auto ShouldUnequipOnRaceChange(const bool equippableByNewRace) -> bool
{
    return !equippableByNewRace;
}

// IsRaceChangeEquipSlotInRange mirrors SLOT_MAIN..SLOT_BACK inclusive.
constexpr auto IsRaceChangeEquipSlotInRange(const std::uint8_t slotId, const std::uint8_t slotMain, const std::uint8_t slotBack) -> bool
{
    return slotId >= slotMain && slotId <= slotBack;
}

// --- HomePoint pure admission ---

// ShouldRejectHomePointAtCap mirrors IsZoneAtPlayerCap.
constexpr auto ShouldRejectHomePointAtCap(const bool zoneAtPlayerCap) -> bool
{
    return zoneAtPlayerCap;
}

// ShouldResetHPMPOnHomePoint mirrors resetHPMP flag.
constexpr auto ShouldResetHPMPOnHomePoint(const bool resetHPMP) -> bool
{
    return resetHPMP;
}

} // namespace entityspawnhelpers
