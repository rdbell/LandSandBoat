#pragma once

#include <cstdint>

namespace spawnslotassignment
{

inline auto shouldAssign(const uint32_t slotId) -> bool
{
    return slotId > 0;
}

} // namespace spawnslotassignment
