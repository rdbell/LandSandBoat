#pragma once

#include "common/cbasetypes.h"

#include "items.h"

#include <array>
#include <string>
#include <unordered_map>
#include <utility>

namespace synthrecipeloadhelpers
{

inline auto CrystalString(const uint16 crystalID) -> const char*
{
    switch (crystalID)
    {
        case FIRE_CRYSTAL:
        case INFERNO_CRYSTAL:
        case PYRE_CRYSTAL:
            return "Fire";
        case ICE_CRYSTAL:
        case GLACIER_CRYSTAL:
        case FROST_CRYSTAL:
            return "Ice";
        case WIND_CRYSTAL:
        case CYCLONE_CRYSTAL:
        case VORTEX_CRYSTAL:
            return "Wind";
        case EARTH_CRYSTAL:
        case TERRA_CRYSTAL:
        case GEO_CRYSTAL:
            return "Earth";
        case LIGHTNING_CRYSTAL:
        case PLASMA_CRYSTAL:
        case BOLT_CRYSTAL:
            return "Lightning";
        case WATER_CRYSTAL:
        case TORRENT_CRYSTAL:
        case FLUID_CRYSTAL:
            return "Water";
        case LIGHT_CRYSTAL:
        case AURORA_CRYSTAL:
        case GLIMMER_CRYSTAL:
            return "Light";
        case DARK_CRYSTAL:
        case TWILIGHT_CRYSTAL:
        case SHADOW_CRYSTAL:
            return "Dark";
        default:
            return "None";
    }
}

inline auto IngredientKey(const uint16 crystal, const std::array<uint16, 8>& ingredients) -> std::string
{
    std::string key = CrystalString(crystal);
    for (const auto ingredient : ingredients)
    {
        key += '-';
        key += std::to_string(ingredient);
    }
    return key;
}

template <typename Recipe>
void StoreByIngredientKey(std::unordered_map<std::string, Recipe>& recipes, const uint16 crystal, const std::array<uint16, 8>& ingredients, Recipe recipe)
{
    recipes[IngredientKey(crystal, ingredients)] = std::move(recipe);
}

} // namespace synthrecipeloadhelpers
