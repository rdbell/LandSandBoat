#pragma once

#include "0x036_talknum.h"

namespace talknumhelpers
{

struct Facts
{
    uint32 uniqueNo{};
    uint16 actIndex{};
    bool   playerCharacter{};
};

[[nodiscard]] constexpr auto PlanFor(const Facts& facts, const uint16 messageID, const bool showName, const uint8 mode) -> GP_SERV_COMMAND_TALKNUM::PacketData
{
    return {
        .UniqueNo = facts.uniqueNo,
        .ActIndex = facts.actIndex,
        .MesNum   = static_cast<uint16>((facts.playerCharacter || !showName) ? messageID + 0x8000 : messageID),
        .Type     = mode,
    };
}

} // namespace talknumhelpers
