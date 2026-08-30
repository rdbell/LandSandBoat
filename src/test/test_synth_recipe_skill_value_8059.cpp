#include "test_synth_recipe_skill_value_8059.h"

#include "map/utils/synth_recipe.h"

#include <array>
#include <iostream>
#include <utility>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synth recipe skill value 8059 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runSynthRecipeSkillValue8059SelfTests() -> bool
{
    const synthutils::SynthRecipe recipe{
        .Wood    = 0,
        .Smith   = 1,
        .Gold    = 127,
        .Cloth   = 128,
        .Leather = 200,
        .Bone    = 254,
        .Alchemy = 255,
        .Cook    = 42,
    };

    const std::array<std::pair<SKILLTYPE, uint16>, 8> expected{
        std::pair{ SKILL_WOODWORKING, uint16{ 0 } },
        std::pair{ SKILL_SMITHING, uint16{ 1 } },
        std::pair{ SKILL_GOLDSMITHING, uint16{ 127 } },
        std::pair{ SKILL_CLOTHCRAFT, uint16{ 128 } },
        std::pair{ SKILL_LEATHERCRAFT, uint16{ 200 } },
        std::pair{ SKILL_BONECRAFT, uint16{ 254 } },
        std::pair{ SKILL_ALCHEMY, uint16{ 255 } },
        std::pair{ SKILL_COOKING, uint16{ 42 } },
    };

    bool ok = true;
    for (const auto& [skill, value] : expected)
    {
        ok = expect(recipe.getSkillValue(skill) == value, "craft skill selects and zero-extends its uint8 field") && ok;
    }

    for (const auto skill : { SKILL_NONE, SKILL_FISHING, SKILL_SYNERGY, static_cast<SKILLTYPE>(255) })
    {
        ok = expect(recipe.getSkillValue(skill) == 0, "non-crafting skill returns zero") && ok;
    }

    return ok;
}
