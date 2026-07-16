#pragma once

#include "0x058_assist.h"

namespace assisthelpers
{

struct Facts
{
    uint32 uniqueNo{};
    uint16 actIndex{};
    bool   hasTarget{};
    uint32 targetID{};
};

[[nodiscard]] constexpr auto PlanFor(const Facts& f) -> GP_SERV_COMMAND_ASSIST::PacketData
{
    return { .UniqueNo = f.uniqueNo, .AssistNo = f.hasTarget ? f.targetID : 0, .ActIndex = f.actIndex };
}

} // namespace assisthelpers
