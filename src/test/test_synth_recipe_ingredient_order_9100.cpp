#include "test_synth_recipe_ingredient_order_9100.h"

#include "map/utils/synth_recipe.h"
#include "omega_self_test_registry.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

namespace
{

auto triggerError(const std::array<std::uint16_t, 8>& ingredients) -> std::string
{
    for (std::size_t index = 1; index < ingredients.size(); ++index)
    {
        if (ingredients[index] > 0 && ingredients[index - 1] > ingredients[index])
        {
            return "[table:synth_recipes] - `Ingredient" + std::to_string(index) + "` is larger than Ingredient" + std::to_string(index + 1);
        }
    }
    return {};
}

auto triggerError(const synthutils::SynthRecipe& recipe) -> std::string
{
    return triggerError({ recipe.Ingredient1, recipe.Ingredient2, recipe.Ingredient3, recipe.Ingredient4,
                           recipe.Ingredient5, recipe.Ingredient6, recipe.Ingredient7, recipe.Ingredient8 });
}

auto expectEqual(const std::string& actual, const std::string& expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "synth recipe ingredient order 9100 self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runSynthRecipeIngredientOrder9100SelfTests() -> bool
{
    bool ok = true;

    ok = expectEqual(triggerError(synthutils::SynthRecipe{}), {}, "empty recipe accepts") && ok;

    synthutils::SynthRecipe ordered{};
    ordered.Ingredient1 = 1;
    ordered.Ingredient2 = 1;
    ordered.Ingredient3 = 2;
    ordered.Ingredient4 = 2;
    ordered.Ingredient5 = 7;
    ordered.Ingredient6 = 7;
    ordered.Ingredient7 = 8;
    ordered.Ingredient8 = 9;
    ok = expectEqual(triggerError(ordered), {}, "ordered ingredients accept") && ok;

    synthutils::SynthRecipe zeroSlot{};
    zeroSlot.Ingredient1 = 10;
    zeroSlot.Ingredient3 = 5;
    ok = expectEqual(triggerError(zeroSlot), {}, "zero current slot suppresses pair") && ok;

    synthutils::SynthRecipe firstFailure{};
    firstFailure.Ingredient1 = 10;
    firstFailure.Ingredient2 = 9;
    firstFailure.Ingredient3 = 8;
    firstFailure.Ingredient4 = 11;
    firstFailure.Ingredient5 = 12;
    firstFailure.Ingredient6 = 13;
    firstFailure.Ingredient7 = 14;
    firstFailure.Ingredient8 = 15;
    ok = expectEqual(triggerError(firstFailure), "[table:synth_recipes] - `Ingredient1` is larger than Ingredient2", "first failure wins") && ok;

    synthutils::SynthRecipe laterFailure{};
    laterFailure.Ingredient1 = 1;
    laterFailure.Ingredient2 = 3;
    laterFailure.Ingredient3 = 2;
    laterFailure.Ingredient4 = 4;
    laterFailure.Ingredient5 = 5;
    laterFailure.Ingredient6 = 6;
    laterFailure.Ingredient7 = 7;
    laterFailure.Ingredient8 = 8;
    ok = expectEqual(triggerError(laterFailure), "[table:synth_recipes] - `Ingredient2` is larger than Ingredient3", "later failure reported") && ok;

    synthutils::SynthRecipe lastFailure{};
    lastFailure.Ingredient1 = 1;
    lastFailure.Ingredient2 = 2;
    lastFailure.Ingredient3 = 3;
    lastFailure.Ingredient4 = 4;
    lastFailure.Ingredient5 = 5;
    lastFailure.Ingredient6 = 6;
    lastFailure.Ingredient7 = 8;
    lastFailure.Ingredient8 = 7;
    ok = expectEqual(triggerError(lastFailure), "[table:synth_recipes] - `Ingredient7` is larger than Ingredient8", "last pair reported") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("synth-recipe-ingredient-order-9100", runSynthRecipeIngredientOrder9100SelfTests);
