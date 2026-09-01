#include "test_synergy_recipe_ingredient_order_9101.h"
#include "omega_self_test_registry.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace
{

auto triggerAccepts(const std::array<std::uint16_t, 8>& ingredients) -> bool
{
    // This mirrors ensure_synergy_ingredients_are_ordered in
    // sql/synergy_recipes.sql: a zero current slot skips that pair.
    for (std::size_t i = 1; i < ingredients.size(); ++i)
    {
        if (ingredients[i] > 0 && ingredients[i - 1] > ingredients[i])
        {
            return false;
        }
    }
    return true;
}

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synergy recipe ingredient order 9101 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runSynergyRecipeIngredientOrder9101SelfTests() -> bool
{
    bool ok = true;

    ok = expect(triggerAccepts({ 1, 2, 3, 4, 5, 6, 7, 8 }), "ascending ingredients are accepted") && ok;
    ok = expect(triggerAccepts({ 9, 9, 9, 9, 0, 0, 0, 0 }), "equal ingredients are accepted") && ok;
    ok = expect(triggerAccepts({ 9, 0, 1, 2, 3, 4, 5, 6 }), "zero current slot skips its pair") && ok;
    ok = expect(triggerAccepts({ 0, 0, 0, 0, 0, 0, 0, 0 }), "empty recipe is accepted") && ok;
    ok = expect(!triggerAccepts({ 2, 1, 3, 4, 5, 6, 7, 8 }), "ingredient pair 1 rejects descending values") && ok;
    ok = expect(!triggerAccepts({ 1, 3, 2, 4, 5, 6, 7, 8 }), "ingredient pair 2 rejects descending values") && ok;
    ok = expect(!triggerAccepts({ 1, 2, 3, 4, 5, 6, 8, 7 }), "ingredient pair 7 rejects descending values") && ok;
    ok = expect(triggerAccepts({ 0, 1, 65535, 65535, 0, 0, 0, 0 }), "uint16 boundary values are accepted") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("synergy-recipe-ingredient-order-9101", runSynergyRecipeIngredientOrder9101SelfTests);
