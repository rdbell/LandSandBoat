#pragma once

#include "common/cbasetypes.h"

namespace zonepacketbroadcast
{

constexpr auto CharacterUpdatePacket = uint16{ 0x00D };
constexpr auto EntityUpdatePacket    = uint16{ 0x00E };
constexpr auto ActionPacket          = uint16{ 0x028 };
constexpr auto EntityDespawnFlag     = uint8{ 0x20 };
constexpr auto SpecialEntityFlag     = uint8{ 0x0F };

// ShouldSuppressHiddenCharacterPacket mirrors PushPacket's early hidden-GM
// return while allowing the character despawn packet through.
inline auto ShouldSuppressHiddenCharacterPacket(const uint16 packetType, const bool sourceIsCharacter,
                                                const bool sourceIsGMHidden, const uint8 entityUpdateFlags) -> bool
{
    return packetType == CharacterUpdatePacket && sourceIsCharacter && sourceIsGMHidden && entityUpdateFlags != EntityDespawnFlag;
}

// ShouldSendRangePacket mirrors the common CHAR_INRANGE recipient admission.
inline auto ShouldSendRangePacket(const bool notSource, const bool inRange, const bool sameMogHouse) -> bool
{
    return notSource && inRange && sameMogHouse;
}

// ShouldSendShoutPacket mirrors the common CHAR_INSHOUT recipient admission.
inline auto ShouldSendShoutPacket(const bool notSource, const bool withinShoutRange, const bool sameMogHouse) -> bool
{
    return notSource && withinShoutRange && sameMogHouse;
}

// ShouldSendZonePacket mirrors CHAR_INZONE's character recipient admission.
inline auto ShouldSendZonePacket(const bool notInMogHouse, const bool notSource) -> bool
{
    return notInMogHouse && notSource;
}

// RequiresSpawnListFilter intentionally retains PushPacket's original
// entity-update expression. Its two inequality tests are joined by ||, so all
// entity-update packets, including 0x20 and 0x0F flags, require a spawn-list
// entry; action packets do too.
inline auto RequiresSpawnListFilter(const uint16 packetType, const uint8 entityUpdateFlags) -> bool
{
    return (packetType == EntityUpdatePacket &&
            (entityUpdateFlags != EntityDespawnFlag || entityUpdateFlags != SpecialEntityFlag)) ||
           packetType == ActionPacket;
}

} // namespace zonepacketbroadcast
