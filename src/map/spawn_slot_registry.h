#pragma once

namespace spawnslotregistry
{

inline auto shouldCreateSlot(const bool slotExists) -> bool
{
    return !slotExists;
}

} // namespace spawnslotregistry
