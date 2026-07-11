#pragma once

#include <cstdint>

namespace worldipc
{

struct KillSessionZones
{
    std::uint32_t previous{};
    std::uint32_t current{};
};

} // namespace worldipc
