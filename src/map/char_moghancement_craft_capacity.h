#pragma once

#include "items/item_furnishing.h"
#include "modifier.h"

#include <cstdint>
#include <functional>
#include <utility>

namespace charmoghancementcrafthelpers
{

template <typename AddModifier>
inline bool Apply(const std::uint16_t moghancementID, const bool isAdding, AddModifier&& addModifier)
{
    const std::int16_t multiplier = isAdding ? 1 : -1;

    if (moghancementID >= MOGHANCEMENT_FIRE && moghancementID <= MOGHANCEMENT_DARK)
    {
        const auto mod = static_cast<Mod>(static_cast<std::uint16_t>(Mod::SYNTH_MATERIAL_LOSS_FIRE) +
                                          moghancementID - MOGHANCEMENT_FIRE);
        std::invoke(std::forward<AddModifier>(addModifier), mod, static_cast<std::int16_t>(5 * multiplier));
        return true;
    }

    if (moghancementID >= MOGHANCEMENT_FISHING && moghancementID <= MOGHANCEMENT_COOKING)
    {
        const auto mod = static_cast<Mod>(static_cast<std::uint16_t>(Mod::FISH) +
                                          moghancementID - MOGHANCEMENT_FISHING);
        std::invoke(std::forward<AddModifier>(addModifier), mod, multiplier);
        return true;
    }

    if (moghancementID >= MOGLIFICATION_FISHING && moghancementID <= MOGLIFICATION_COOKING)
    {
        const auto offset   = moghancementID - MOGLIFICATION_FISHING;
        const auto skillMod = static_cast<Mod>(static_cast<std::uint16_t>(Mod::FISH) + offset);
        std::invoke(addModifier, skillMod, multiplier);
        if (offset != 0)
        {
            const auto lossMod = static_cast<Mod>(static_cast<std::uint16_t>(Mod::SYNTH_MATERIAL_LOSS_WOODWORKING) + offset - 1);
            std::invoke(addModifier, lossMod, static_cast<std::int16_t>(5 * multiplier));
        }
        return true;
    }

    if (moghancementID >= MEGA_MOGLIFICATION_FISHING && moghancementID <= MEGA_MOGLIFICATION_COOKING)
    {
        const auto mod = static_cast<Mod>(static_cast<std::uint16_t>(Mod::FISH) +
                                          moghancementID - MEGA_MOGLIFICATION_FISHING);
        std::invoke(std::forward<AddModifier>(addModifier), mod, static_cast<std::int16_t>(5 * multiplier));
        return true;
    }

    return false;
}

} // namespace charmoghancementcrafthelpers
