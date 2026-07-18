#pragma once

namespace targetfindlockhelpers
{

// ShouldRejectAIOrLocked mirrors validEntity's AI untargetable and character
// lock gates. A lock only applies when the candidate is a character.
inline auto ShouldRejectAIOrLocked(const bool aiUntargetable, const bool isCharacter, const bool characterLocked) -> bool
{
    return aiUntargetable || (isCharacter && characterLocked);
}

} // namespace targetfindlockhelpers
