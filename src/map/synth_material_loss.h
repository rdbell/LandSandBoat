#pragma once

#include "common/cbasetypes.h"

#include <algorithm>

namespace synthmateriallosshelpers
{

enum class IngredientOutcome : uint8
{
    Break,
    Save,
};

constexpr auto CalculateBreakChance(int16 globalReduction, int16 elementalReduction, int16 typeReduction, int16 difficulty) -> int16
{
    if (difficulty < 0)
    {
        difficulty = 0;
    }
    const auto chance = 50 - static_cast<int>(globalReduction) - static_cast<int>(elementalReduction) -
                        static_cast<int>(typeReduction) + 5 * static_cast<int>(difficulty);
    return static_cast<int16>(std::clamp(chance, 20, 100));
}

constexpr auto ResolveIngredient(const uint8 randomZeroToNinetyNine, const int16 breakChance) -> IngredientOutcome
{
    return 1 + randomZeroToNinetyNine <= breakChance ? IngredientOutcome::Break : IngredientOutcome::Save;
}

} // namespace synthmateriallosshelpers
