#pragma once

namespace automatoncontrollermasterenhancementtarget
{
inline auto CanConsiderMasterEnhancement(bool withinRange, bool targetIsMob, bool masterHasEnmity) -> bool
{
    return withinRange && targetIsMob && masterHasEnmity;
}
} // namespace automatoncontrollermasterenhancementtarget
