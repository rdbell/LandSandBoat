#pragma once

#include "modifier.h"

#include <cstdint>

// Pure battleutils::GetScaledItemModifier after null checks and injects.
// Parity: internal/itemequipment.ScaledItemModifier

namespace scaleditemmodifierhelpers
{

constexpr auto ShouldRejectNull(const bool entityNull, const bool itemNull) -> bool
{
    return entityNull || itemNull;
}

// mLevel / reqLvl / amount inject; mod is LSB Mod enum.
constexpr auto ScaledItemModifier(const std::uint8_t mLevel, const std::uint8_t reqLvl, const Mod mod, const std::int16_t amount) -> std::int32_t
{
    if (mLevel >= reqLvl)
    {
        return static_cast<std::int32_t>(amount);
    }

    // Promote like production (int16 amount used in *= / /= with int promotion).
    auto modAmount = static_cast<std::int32_t>(amount);
    switch (mod)
    {
        case Mod::DEF:
        case Mod::MAIN_DMG_RATING:
        case Mod::SUB_DMG_RATING:
        case Mod::RANGED_DMG_RATING:
            modAmount *= 3;
            modAmount /= 4;
            break;
        case Mod::HP:
        case Mod::MP:
            modAmount /= 2;
            break;
        case Mod::STR:
        case Mod::DEX:
        case Mod::VIT:
        case Mod::AGI:
        case Mod::INT:
        case Mod::MND:
        case Mod::CHR:
        case Mod::ATT:
        case Mod::RATT:
        case Mod::ACC:
        case Mod::RACC:
        case Mod::MATT:
        case Mod::MACC:
            modAmount /= 3;
            break;
        default:
            modAmount = 0;
            break;
    }
    return modAmount / static_cast<std::int32_t>(reqLvl);
}

} // namespace scaleditemmodifierhelpers
