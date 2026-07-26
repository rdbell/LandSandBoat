#include "test_synth_recipe_load_7080.h"

#include "map/synth_recipe_load.h"

#include <array>
#include <iostream>
#include <string>
#include <unordered_map>

auto runSynthRecipeLoad7080SelfTests() -> bool
{
    const std::array<uint16, 8>             ingredients{ 10, 20, 30, 40, 50, 60, 70, 80 };
    std::unordered_map<std::string, uint32> recipes;
    synthrecipeloadhelpers::StoreByIngredientKey(recipes, FIRE_CRYSTAL, ingredients, uint32{ 100 });
    synthrecipeloadhelpers::StoreByIngredientKey(recipes, INFERNO_CRYSTAL, ingredients, uint32{ 200 });
    synthrecipeloadhelpers::StoreByIngredientKey(recipes, FIRE_CRYSTAL, ingredients, uint32{ 300 });

    const bool ok = synthrecipeloadhelpers::IngredientKey(FIRE_CRYSTAL, ingredients) == "Fire-10-20-30-40-50-60-70-80" &&
                    synthrecipeloadhelpers::IngredientKey(INFERNO_CRYSTAL, ingredients) == "Fire-10-20-30-40-50-60-70-80" &&
                    synthrecipeloadhelpers::IngredientKey(0, ingredients) == "None-10-20-30-40-50-60-70-80" &&
                    recipes.size() == 1 && recipes[synthrecipeloadhelpers::IngredientKey(FIRE_CRYSTAL, ingredients)] == 300;
    if (!ok)
    {
        std::cerr << "synth recipe load 7080 self-test failed\n";
    }
    return ok;
}
