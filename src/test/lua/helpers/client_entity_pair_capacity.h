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

// tradeNpc max items (itemCount > 9 rejects).
inline constexpr int TradeMaxItems = 9;

// craft ingredient count bounds.
inline constexpr int CraftMinIngredients = 1;
inline constexpr int CraftMaxIngredients = 8;

// moveItem dstSlot default.
inline constexpr uint8_t MoveItemDefaultDstSlot = 0xFF;

// bcnm:expectWin / enter event ids and delayed skip.
inline constexpr uint16_t BCNMExpectWinEventID     = 32001;
inline constexpr uint16_t BCNMEnterEventID         = 32000;
inline constexpr int      BCNMDelayedWinSkipSeconds = 7;

// enter option = (bcnmIndex << 4) + 1
inline auto BCNMEnterOption(const uint16_t bcnmIndex) -> uint32_t
{
    return (static_cast<uint32_t>(bcnmIndex) << 4) + 1;
}

// tradeNpc item count admission (after npc found).
inline auto TradeItemCountOK(const int itemCount) -> bool
{
    return itemCount > 0 && itemCount <= TradeMaxItems;
}

// craft ingredient count admission (after crystal found).
inline auto CraftIngredientCountOK(const int ingredientCount) -> bool
{
    return ingredientCount >= CraftMinIngredients && ingredientCount <= CraftMaxIngredients;
}

// skillchain needs ≥2 weaponskill ids.
inline auto SkillchainWSCountOK(const int wsCount) -> bool
{
    return wsCount >= 2;
}

} // namespace cliententitypairhelpers
