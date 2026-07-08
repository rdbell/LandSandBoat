/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_craft_state.h"

#include "items/craft_state.h"

#include <iostream>

namespace
{

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "craft state self-test failed: " << label << " got "
                  << static_cast<int>(actual) << " expected " << static_cast<int>(expected) << '\n';
        return false;
    }
    return true;
}

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "craft state self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectResult(const CCraftState::Result& actual, const CCraftState::Result& expected, const char* label) -> bool
{
    bool ok = true;
    ok      = expectEqual(actual.itemId, expected.itemId, label) && ok;
    ok      = expectEqual(actual.qty, expected.qty, label) && ok;
    return ok;
}

auto makeInit() -> CCraftState::Init
{
    CCraftState::Init data{};
    data.recipeId      = 123456;
    data.craftMode     = CRAFT_DESYNTHESIS;
    data.crystalItemId = 4096;
    data.element       = 5;
    data.results       = {
        CCraftState::Result{ 1001, 1 },
        CCraftState::Result{ 1002, 2 },
        CCraftState::Result{ 1003, 3 },
        CCraftState::Result{ 1004, 4 },
        CCraftState::Result{ 1005, 5 },
    };
    data.skillRequired     = { 10, 20, 30, 40, 50, 60, 70, 80 };
    data.ingredientItemIds = { 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 };
    return data;
}

auto testCraftStateDefaults() -> bool
{
    CCraftState state;

    bool ok = true;
    ok      = expectEqual(state.recipeId(), static_cast<uint32>(0), "default recipe id") && ok;
    ok      = expectEqual(state.craftMode(), CRAFT_SYNTHESIS, "default craft mode") && ok;
    ok      = expectEqual(state.crystalItemId(), static_cast<uint16>(0), "default crystal item id") && ok;
    ok      = expectEqual(state.element(), static_cast<uint8>(0), "default element") && ok;
    ok      = expectEqual(state.failingSkill(), static_cast<uint8>(0), "default failing skill") && ok;
    ok      = expectEqual(state.result(), static_cast<uint8>(0), "default result") && ok;
    for (uint8 idx = 0; idx < SynthMaxIngredients; ++idx)
    {
        ok = expectEqual(state.skillRequired(idx), static_cast<uint8>(0), "default skill required") && ok;
        ok = expectEqual(state.ingredientItemId(idx), static_cast<uint16>(0), "default ingredient item id") && ok;
        ok = expectBool(state.isBroken(idx), false, "default ingredient broken") && ok;
    }
    ok = expectResult(state.resultTier(0), CCraftState::Result{}, "default result tier") && ok;
    return ok;
}

auto testCraftStatePopulateAndMutate() -> bool
{
    CCraftState state;
    auto        data = makeInit();

    state.populate(data);

    bool ok = true;
    ok      = expectEqual(state.recipeId(), static_cast<uint32>(123456), "recipe id") && ok;
    ok      = expectEqual(state.craftMode(), CRAFT_DESYNTHESIS, "craft mode") && ok;
    ok      = expectEqual(state.crystalItemId(), static_cast<uint16>(4096), "crystal item id") && ok;
    ok      = expectEqual(state.element(), static_cast<uint8>(5), "element") && ok;
    ok      = expectResult(state.resultTier(2), CCraftState::Result{ 1003, 3 }, "result tier") && ok;
    ok      = expectEqual(state.skillRequired(6), static_cast<uint8>(70), "skill required") && ok;
    ok      = expectEqual(state.ingredientItemId(7), static_cast<uint16>(2008), "ingredient item id") && ok;
    ok      = expectEqual(state.failingSkill(), static_cast<uint8>(0), "populate resets failing skill") && ok;
    ok      = expectEqual(state.result(), static_cast<uint8>(0), "populate resets result") && ok;
    ok      = expectBool(state.isBroken(3), false, "populate resets broken ingredient") && ok;

    state.setFailingSkill(4);
    state.setResult(2);
    state.markBroken(3);

    ok = expectEqual(state.failingSkill(), static_cast<uint8>(4), "set failing skill") && ok;
    ok = expectEqual(state.result(), static_cast<uint8>(2), "set result") && ok;
    ok = expectBool(state.isBroken(3), true, "mark broken") && ok;
    ok = expectBool(state.isBroken(2), false, "unmarked ingredient remains unbroken") && ok;

    data.recipeId      = 654321;
    data.craftMode     = CRAFT_SYNTHESIS_NO_LOSS;
    data.crystalItemId = 8192;
    data.element       = 7;
    data.results[2]    = CCraftState::Result{ 9000, 9 };
    state.populate(data);

    ok = expectEqual(state.recipeId(), static_cast<uint32>(654321), "second recipe id") && ok;
    ok = expectEqual(state.craftMode(), CRAFT_SYNTHESIS_NO_LOSS, "second craft mode") && ok;
    ok = expectResult(state.resultTier(2), CCraftState::Result{ 9000, 9 }, "second result tier") && ok;
    ok = expectEqual(state.failingSkill(), static_cast<uint8>(0), "second populate resets failing skill") && ok;
    ok = expectEqual(state.result(), static_cast<uint8>(0), "second populate resets result") && ok;
    ok = expectBool(state.isBroken(3), false, "second populate resets broken ingredient") && ok;
    return ok;
}

} // namespace

auto runCraftStateSelfTests() -> bool
{
    bool ok = true;
    ok      = testCraftStateDefaults() && ok;
    ok      = testCraftStatePopulateAndMutate() && ok;
    return ok;
}
