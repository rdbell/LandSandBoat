/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_recipe_runtime.h"

#include <array>
#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x058_recipe.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s RECIPE runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

struct TestCase
{
    GP_CLI_COMMAND_RECIPE_MODE  mode;
    GP_SERV_COMMAND_RECIPE_TYPE type;
    uint8                       skillRank;
    uint16                      offset;
};

auto testResponsePlans() -> bool
{
    constexpr auto skill  = uint16{ 5 };
    constexpr auto level  = uint16{ 87 };
    constexpr auto param1 = uint16{ 32 };
    constexpr auto param3 = uint16{ 7 };
    constexpr auto param4 = uint16{ 8 };
    constexpr auto cases  = std::array{
        TestCase{ GP_CLI_COMMAND_RECIPE_MODE::RequestAvailableRankList, GP_SERV_COMMAND_RECIPE_TYPE::RecipeDetail1, 0, 0 },
        TestCase{ GP_CLI_COMMAND_RECIPE_MODE::RequestAvailableRecipeList, GP_SERV_COMMAND_RECIPE_TYPE::RecipeList, static_cast<uint8>(param4), param1 },
        TestCase{ GP_CLI_COMMAND_RECIPE_MODE::RequestRecipeMaterials, GP_SERV_COMMAND_RECIPE_TYPE::RecipeDetail2, static_cast<uint8>(param4), param3 },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        const auto response = recipehelpers::BuildResponsePlan(static_cast<uint16>(test.mode), skill, level, param1, param3, param4);
        ok                  = expect(response.has_value(), "supported mode has a response") && ok;
        if (!response)
        {
            continue;
        }
        ok = expect(response->type == test.type, "response type") && ok;
        ok = expect(response->skill == skill, "skill propagated") && ok;
        ok = expect(response->level == level, "level propagated") && ok;
        ok = expect(response->skillRank == test.skillRank, "skill rank selected") && ok;
        ok = expect(response->offset == test.offset, "offset selected") && ok;
    }
    return ok;
}

auto testUnsupportedModeHasNoResponse() -> bool
{
    return expect(!recipehelpers::BuildResponsePlan(static_cast<uint16>(GP_CLI_COMMAND_RECIPE_MODE::RequestCampaignOpsRecipe), 5, 87, 32, 7, 8).has_value(), "unsupported mode has no response");
}

} // namespace

auto runC2SRecipeRuntimeSelfTests() -> bool
{
    return testResponsePlans() && testUnsupportedModeHasNoResponse();
}
