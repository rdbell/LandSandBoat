#pragma once

#include "common/cbasetypes.h"

namespace synthreciperesolvehelpers
{

enum class Decision
{
    Continue,
    CancelBadRecipe,
    CancelRareItem,
    CancelSkillTooLow,
};

constexpr auto CheckFound(const bool found) -> Decision
{
    return found ? Decision::Continue : Decision::CancelBadRecipe;
}

constexpr auto CheckContent(const bool contentEnabled) -> Decision
{
    return contentEnabled ? Decision::Continue : Decision::CancelBadRecipe;
}

constexpr auto CheckRareItem(const bool resultIsRare, const bool alreadyOwned) -> Decision
{
    return resultIsRare && alreadyOwned ? Decision::CancelRareItem : Decision::Continue;
}

constexpr auto CheckRequiredKeyItem(const bool requiredKeyItemMissing) -> Decision
{
    return requiredKeyItemMissing ? Decision::CancelBadRecipe : Decision::Continue;
}

constexpr auto CheckSkill(const uint16 currentSkillTenths, const uint8 requiredSkill) -> Decision
{
    return static_cast<int32>(currentSkillTenths) < static_cast<int32>(requiredSkill) * 10 - 150 ? Decision::CancelSkillTooLow : Decision::Continue;
}

} // namespace synthreciperesolvehelpers
