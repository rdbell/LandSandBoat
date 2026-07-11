#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure createCharacter field validation, starting-job clamp, nation/zone
// selection, next-char-id, and char-info packing helpers extracted so native
// tests can pin policy without SQL/random hosts.

namespace loginHelpers
{

// Packet field offsets into the VIEW 0x21 register buffer used by createCharacter.
constexpr std::size_t CreateCharRaceOffset   = 48;
constexpr std::size_t CreateCharJobOffset    = 50;
constexpr std::size_t CreateCharNationOffset = 54;
constexpr std::size_t CreateCharSizeOffset   = 57;
constexpr std::size_t CreateCharFaceOffset   = 60;

// Race: 1 (HumeM) .. 8 (Galka).
constexpr uint8 CreateCharRaceMin = 1;
constexpr uint8 CreateCharRaceMax = 8;

// Size: 0..2 (Large).
constexpr uint8 CreateCharSizeMax = 2;

// Face: 0..15 (Face 8B).
constexpr uint8 CreateCharFaceMax = 15;

// Starting jobs clamped to 1..6.
constexpr uint8 CreateCharStartingJobMin = 1;
constexpr uint8 CreateCharStartingJobMax = 6;

// Nation: 0 San d'Oria, 1 Bastok, 2 Windurst.
constexpr uint8 CreateCharNationMax = 2;

// Starting zone IDs by nation (three choices each; index selected by host RNG).
inline constexpr std::array<uint32, 3> SandoriaStartingZones = { 0xE6, 0xE7, 0xE8 };
inline constexpr std::array<uint32, 3> BastokStartingZones   = { 0xEA, 0xEB, 0xEC };
inline constexpr std::array<uint32, 3> WindurstStartingZones = { 0xEE, 0xF0, 0xF1 };

// create_char_field_gate is the pure outcome of race/size/face/nation validation.
enum class create_char_field_gate : uint8_t
{
    ALLOW,
    INVALID_RACE,
    INVALID_SIZE,
    INVALID_FACE,
    INVALID_NATION,
};

// ClassifyCreateCharFields mirrors the sequential field checks in createCharacter
// (race, then size, then face; nation is checked after job clamp).
inline auto ClassifyCreateCharRace(const uint8 race) -> create_char_field_gate
{
    if (race < CreateCharRaceMin || race > CreateCharRaceMax)
    {
        return create_char_field_gate::INVALID_RACE;
    }
    return create_char_field_gate::ALLOW;
}

inline auto ClassifyCreateCharSize(const uint8 size) -> create_char_field_gate
{
    if (size > CreateCharSizeMax)
    {
        return create_char_field_gate::INVALID_SIZE;
    }
    return create_char_field_gate::ALLOW;
}

inline auto ClassifyCreateCharFace(const uint8 face) -> create_char_field_gate
{
    if (face > CreateCharFaceMax)
    {
        return create_char_field_gate::INVALID_FACE;
    }
    return create_char_field_gate::ALLOW;
}

inline auto ClassifyCreateCharNation(const uint8 nation) -> create_char_field_gate
{
    if (nation > CreateCharNationMax)
    {
        return create_char_field_gate::INVALID_NATION;
    }
    return create_char_field_gate::ALLOW;
}

// ClampStartingJob mirrors std::clamp(mjob, 1, 6).
inline auto ClampStartingJob(const uint8 mjob) -> uint8
{
    return std::clamp(mjob, CreateCharStartingJobMin, CreateCharStartingJobMax);
}

// Format helpers for createCharacter diagnostics (name is already sanitized text).
inline auto FormatInvalidCreateRace(const std::string& charName, const uint8 race) -> std::string
{
    return fmt::format("{} attempted to create character with invalid race {}", charName, race);
}

inline auto FormatInvalidCreateSize(const std::string& charName, const uint8 size) -> std::string
{
    return fmt::format("{} attempted to create character with invalid size {}", charName, size);
}

inline auto FormatInvalidCreateFace(const std::string& charName, const uint8 face) -> std::string
{
    return fmt::format("{} attempted to create character with invalid face {}", charName, face);
}

inline auto FormatInvalidCreateNation(const std::string& charName, const uint8 nation) -> std::string
{
    return fmt::format("{} attempted to create character with invalid nation {}", charName, nation);
}

inline auto FormatInvalidStartingJobSubstitution(const std::string& charName, const uint8 requested, const uint8 substituted) -> std::string
{
    return fmt::format("{} attempted to create invalid starting job {} substituting {}", charName, requested, substituted);
}

// StartingZoneForNation picks one of the three nation starting zones by index.
// index is modulo 3 so hosts may pass raw RNG output (LSB uses GetRandomNumber(3)).
inline auto StartingZoneForNation(const uint8 nation, const uint8 index) -> uint32
{
    const auto i = static_cast<std::size_t>(index % 3);
    switch (nation)
    {
        case 0x02:
            return WindurstStartingZones[i];
        case 0x01:
            return BastokStartingZones[i];
        case 0x00:
        default:
            return SandoriaStartingZones[i];
    }
}

// NextCharacterID mirrors MAX(charid)+1 with no floor (unlike account IDs).
// When the table is empty, maxExistingID is 0 → next is 1.
inline auto NextCharacterID(const uint32 maxExistingID) -> uint32
{
    return maxExistingID + 1;
}

// create_char_info_ids is the pure packing of client-facing IDs after save.
struct create_char_info_ids
{
    uint32 ffxi_id{};           // contentId (reuses charID)
    uint16 ffxi_id_world{};     // charID low 16 bits
    uint8  worldid{};           // multi-world stub (0)
    uint8  status{};            // 1 = Available
    uint8  race_change{};       // 0
    uint8  renamef{};           // 0
    uint8  ffxi_id_world_tbl{}; // charID bits 16..23
};

// PackCreateCharInfoIDs mirrors the post-save charInfo ID fields in createCharacter.
inline auto PackCreateCharInfoIDs(const uint32 charID) -> create_char_info_ids
{
    return create_char_info_ids{
        .ffxi_id           = charID,
        .ffxi_id_world     = static_cast<uint16>(charID & 0xFFFF),
        .worldid           = 0,
        .status            = 1,
        .race_change       = 0,
        .renamef           = 0,
        .ffxi_id_world_tbl = static_cast<uint8>((charID >> 16) & 0xFF),
    };
}

} // namespace loginHelpers
