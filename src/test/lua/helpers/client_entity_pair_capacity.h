#pragma once

#include <cstdint>

// Pure dual-wire residuals for CLuaClientEntityPair helper modules (slice 6380).
// Go: internal/cliententitypair (PlanSendEventPacket, ClassifyEntityQuery, etc.).
namespace cliententitypairhelpers
{

// Idle currentEvent->eventId sentinel (uint16(-1)).
inline constexpr uint16_t NoEventID = 65535;

// GP_CLI_COMMAND_EVENTEND_MODE
inline constexpr uint16_t EventEndModeEnd           = 0;
inline constexpr uint16_t EventEndModeUpdatePending = 1;

inline auto InEvent(const uint16_t currentEventID) -> bool
{
    return currentEventID != NoEventID;
}

// Entity query type classes for entities:get switch.
enum class EntityQueryKind : uint8_t
{
    Invalid = 0,
    ID      = 1,
    Name    = 2,
    Entity  = 3,
};

inline auto ClassifyEntityQuery(const bool isNumber, const bool isString, const bool isUserdata) -> EntityQueryKind
{
    if (isNumber)
    {
        return EntityQueryKind::ID;
    }
    if (isString)
    {
        return EntityQueryKind::Name;
    }
    if (isUserdata)
    {
        return EntityQueryKind::Entity;
    }
    return EntityQueryKind::Invalid;
}

// setBlueSpells page capacity and spell id offset.
inline constexpr int    BlueSpellPageSlots = 20;
inline constexpr uint16_t BlueSpellIDOffset = 0x200;

// POS packet dir default when rot optional absent.
inline auto MoveDir(const bool rotPresent, const uint8_t rot) -> uint8_t
{
    return rotPresent ? rot : 0;
}

// FFI body copy starts after GP_CLI_HEADER (4 bytes).
inline constexpr int PacketHeaderSize = 4;

inline auto FFIBodyCopyOK(const int packetSize) -> bool
{
    return packetSize > PacketHeaderSize;
}

} // namespace cliententitypairhelpers
