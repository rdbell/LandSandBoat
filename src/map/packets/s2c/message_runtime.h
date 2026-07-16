#pragma once

#include <string_view>

#include "0x009_message.h"
#include "enums/msg_std.h"

namespace messagehelpers
{

constexpr std::size_t CharacterMessageDataSize = 24;

struct CharacterFacts
{
    bool             present{};
    uint32           uniqueNo{};
    uint16           actIndex{};
    std::string_view name{};
};

struct Plan
{
    uint16                              size{};
    GP_SERV_COMMAND_MESSAGE::PacketData packet{};
};

[[nodiscard]] inline auto CharacterParams2PlanFor(const CharacterFacts& character, const uint32 param0, const uint32 param1, const MsgStd messageID) -> Plan
{
    auto plan         = Plan{};
    plan.size         = 0x24;
    plan.packet.MesNo = static_cast<uint16>(messageID);

    if (!character.present)
    {
        std::snprintf(plan.packet.Data, 24, "Para0 %u Para1 %u", param0, param1);
        return plan;
    }

    plan.packet.UniqueNo = character.uniqueNo;
    plan.packet.ActIndex = character.actIndex;
    const auto setName   = [&plan, &character]
    {
        constexpr std::string_view prefix = "string2 ";
        auto                       index  = std::size_t{};
        for (const auto character : prefix)
        {
            plan.packet.Data[index++] = character;
        }
        for (const auto character : character.name)
        {
            if (character == '\0' || index + 1 >= CharacterMessageDataSize)
            {
                break;
            }
            plan.packet.Data[index++] = character;
        }
    };
    if (messageID == MsgStd::Examine)
    {
        plan.size        = 0x60;
        plan.packet.Attr = 0x10;
        setName();
    }
    else if (messageID == MsgStd::MonstrosityCheckIn || messageID == MsgStd::MonstrosityCheckOut)
    {
        plan.size = 0x20;
        setName();
    }
    return plan;
}

} // namespace messagehelpers
