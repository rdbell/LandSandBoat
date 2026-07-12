#pragma once

#include "items/item_furnishing.h"
#include "modifier.h"

#include <cstdint>
#include <functional>

namespace charmoghancementresistancehelpers
{

template <typename AddModifier>
inline bool Apply(const std::uint16_t moghancementID, const bool isAdding, AddModifier&& addModifier)
{
    Mod mod{};
    switch (moghancementID)
    {
        case MOGLIFICATION_RESIST_DEATH:
            mod = Mod::DEATHRES;
            break;
        case MOGLIFICATION_RESIST_SLEEP:
            mod = Mod::SLEEPRES;
            break;
        case MOGLIFICATION_RESIST_POISON:
            mod = Mod::POISONRES;
            break;
        case MOGLIFICATION_RESIST_PARALYSIS:
            mod = Mod::PARALYZERES;
            break;
        case MOGLIFICATION_RESIST_SILENCE:
            mod = Mod::SILENCERES;
            break;
        case MOGLIFICATION_RESIST_PETRIFICATION:
            mod = Mod::PETRIFYRES;
            break;
        case MOGLIFICATION_RESIST_VIRUS:
            mod = Mod::VIRUSRES;
            break;
        case MOGLIFICATION_RESIST_CURSE:
            mod = Mod::CURSERES;
            break;
        default:
            return false;
    }

    std::invoke(std::forward<AddModifier>(addModifier), mod, static_cast<std::int16_t>(isAdding ? 10 : -10));
    return true;
}

} // namespace charmoghancementresistancehelpers
