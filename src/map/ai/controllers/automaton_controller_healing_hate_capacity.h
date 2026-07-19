#pragma once

namespace automatoncontrollerhealinghate
{
inline auto HasHealingHate(bool targetIsMob, bool masterHasEnmity, bool automatonHasEnmity, int masterEnmity, int automatonEnmity) -> bool
{
    if (!targetIsMob)
    {
        return false;
    }
    if (!masterHasEnmity)
    {
        return true;
    }
    if (!automatonHasEnmity)
    {
        return false;
    }
    return automatonEnmity > masterEnmity;
}
} // namespace automatoncontrollerhealinghate
