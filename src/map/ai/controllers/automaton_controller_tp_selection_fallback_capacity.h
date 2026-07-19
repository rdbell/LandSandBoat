#pragma once

namespace automatoncontrollertpselectionfallback
{
inline auto ShouldUseNormalTPSkillSelection(bool attemptChain, int currentManeuvers, bool masterPresent, int masterTP, int tpEfficiency) -> bool
{
    return !attemptChain || (currentManeuvers == -1 && masterPresent && masterTP < tpEfficiency);
}
} // namespace automatoncontrollertpselectionfallback
