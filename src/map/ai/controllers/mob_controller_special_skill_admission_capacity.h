#pragma once

namespace mobcontrollerspecialskilladmission
{
inline auto CanAttempt(bool hasSkill, bool weaponSkillEnabled, bool hiddenFlag, bool nameHidden) -> bool
{
    return hasSkill && weaponSkillEnabled && (!hiddenFlag || nameHidden);
}
} // namespace mobcontrollerspecialskilladmission
