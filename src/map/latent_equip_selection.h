#pragma once

#include <cstdint>

// Pure CLatentEffectContainer::CheckLatentsEquip selection predicate.
namespace latenthelpers
{

constexpr auto ShouldProcessEquipLatent(const std::uint8_t latentSlot, const std::uint8_t changedSlot) -> bool
{
    return latentSlot == changedSlot;
}

} // namespace latenthelpers
