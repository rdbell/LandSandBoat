#pragma once

#include "common/cbasetypes.h"

namespace charsessionupdatehelpers
{

struct Plan
{
    uint16 targetId{};
    uint32 serverAddress{};
    uint16 clientPort{};
    uint32 characterId{};

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto MakePlan(const uint16 targetId, const uint32 serverAddress, const uint16 clientPort, const uint32 characterId) -> Plan
{
    return { targetId, serverAddress, clientPort, characterId };
}

} // namespace charsessionupdatehelpers
