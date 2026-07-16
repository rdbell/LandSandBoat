#pragma once
#include "0x05c_pendingnum.h"
#include <array>
#include <utility>
#include <vector>

namespace pendingnumhelpers
{

[[nodiscard]] inline auto PlanFor(const std::vector<std::pair<uint8, uint32>>& params) -> GP_SERV_COMMAND_PENDINGNUM::PacketData
{
    auto p = GP_SERV_COMMAND_PENDINGNUM::PacketData{};
    for (const auto& [index, value] : params)
        if (index < std::size(p.num))
            p.num[index] = static_cast<int32>(value);
    return p;
}

} // namespace pendingnumhelpers
