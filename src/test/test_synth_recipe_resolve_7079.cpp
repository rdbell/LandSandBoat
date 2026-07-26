#include "test_synth_recipe_resolve_7079.h"

#include "map/synth_recipe_resolve.h"

#include <iostream>

auto runSynthRecipeResolve7079SelfTests() -> bool
{
    using synthreciperesolvehelpers::CheckContent;
    using synthreciperesolvehelpers::CheckFound;
    using synthreciperesolvehelpers::CheckRareItem;
    using synthreciperesolvehelpers::CheckRequiredKeyItem;
    using synthreciperesolvehelpers::CheckSkill;
    using synthreciperesolvehelpers::Decision;

    const bool ok = CheckFound(false) == Decision::CancelBadRecipe &&
                    CheckContent(false) == Decision::CancelBadRecipe &&
                    CheckRareItem(true, true) == Decision::CancelRareItem &&
                    CheckRareItem(true, false) == Decision::Continue &&
                    CheckRequiredKeyItem(true) == Decision::CancelBadRecipe &&
                    CheckSkill(0, 0) == Decision::Continue &&
                    CheckSkill(849, 100) == Decision::CancelSkillTooLow &&
                    CheckSkill(850, 100) == Decision::Continue;
    if (!ok)
    {
        std::cerr << "synth recipe resolve 7079 self-test failed\n";
    }
    return ok;
}
