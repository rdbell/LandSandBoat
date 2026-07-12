#pragma once

#include "items/item_furnishing.h"
#include "modifier.h"

#include <cstdint>
#include <functional>

namespace charmoghancementgeneralhelpers
{

template <typename AddModifier>
inline bool Apply(const std::uint16_t moghancementID, const std::uint8_t nation, const bool isAdding, AddModifier&& addModifier)
{
    const std::int16_t multiplier = isAdding ? 1 : -1;

    switch (moghancementID)
    {
        case MOGHANCEMENT_EXPERIENCE:
            std::invoke(addModifier, Mod::EXPERIENCE_RETAINED, static_cast<std::int16_t>(5 * multiplier));
            return true;
        case MOGHANCEMENT_GARDENING:
            std::invoke(addModifier, Mod::GARDENING_WILT_BONUS, static_cast<std::int16_t>(36 * multiplier));
            return true;
        case MOGHANCEMENT_DESYNTHESIS:
            std::invoke(addModifier, Mod::SYNTH_SUCCESS_RATE_DESYNTHESIS, static_cast<std::int16_t>(2 * multiplier));
            return true;
        case MOGHANCEMENT_CONQUEST:
            std::invoke(addModifier, Mod::CONQUEST_BONUS, static_cast<std::int16_t>(6 * multiplier));
            return true;
        case MOGHANCEMENT_REGION:
            std::invoke(addModifier, Mod::CONQUEST_REGION_BONUS, static_cast<std::int16_t>(10 * multiplier));
            return true;
        case MOGHANCEMENT_FISHING_ITEM:
            return true;
        case MOGHANCEMENT_SANDORIA_CONQUEST:
        case MOGHANCEMENT_BASTOK_CONQUEST:
        case MOGHANCEMENT_WINDURST_CONQUEST:
            if (nation == moghancementID - MOGHANCEMENT_SANDORIA_CONQUEST)
            {
                std::invoke(addModifier, Mod::CONQUEST_BONUS, static_cast<std::int16_t>(6 * multiplier));
            }
            return true;
        case MOGHANCEMENT_MONEY:
            std::invoke(addModifier, Mod::MOGHANCEMENT_GIL_BONUS_P, static_cast<std::int16_t>(10 * multiplier));
            return true;
        case MOGHANCEMENT_CAMPAIGN:
            std::invoke(addModifier, Mod::CAMPAIGN_BONUS, static_cast<std::int16_t>(5 * multiplier));
            return true;
        case MOGHANCEMENT_MONEY_II:
            std::invoke(addModifier, Mod::MOGHANCEMENT_GIL_BONUS_P, static_cast<std::int16_t>(15 * multiplier));
            return true;
        case MOGHANCEMENT_SKILL_GAINS:
            std::invoke(addModifier, Mod::COMBAT_SKILLUP_RATE, static_cast<std::int16_t>(25 * multiplier));
            std::invoke(addModifier, Mod::MAGIC_SKILLUP_RATE, static_cast<std::int16_t>(25 * multiplier));
            return true;
        case MOGHANCEMENT_BOUNTY:
            std::invoke(addModifier, Mod::EXP_BONUS, static_cast<std::int16_t>(10 * multiplier));
            std::invoke(addModifier, Mod::CAPACITY_BONUS, static_cast<std::int16_t>(10 * multiplier));
            return true;
        case MOGLIFICATION_EXPERIENCE_BOOST:
            std::invoke(addModifier, Mod::EXP_BONUS, static_cast<std::int16_t>(15 * multiplier));
            return true;
        case MOGLIFICATION_CAPACITY_BOOST:
            std::invoke(addModifier, Mod::CAPACITY_BONUS, static_cast<std::int16_t>(15 * multiplier));
            return true;
        default:
            return false;
    }
}

} // namespace charmoghancementgeneralhelpers
