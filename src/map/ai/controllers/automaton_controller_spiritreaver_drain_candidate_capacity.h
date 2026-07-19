#pragma once

namespace automatoncontrollerspiritreaverdraincandidate
{
inline auto CanSelectCandidate(uint8 automatonHPP, bool targetUndead) -> bool
{
    return automatonHPP < 75 && !targetUndead;
}
} // namespace automatoncontrollerspiritreaverdraincandidate
