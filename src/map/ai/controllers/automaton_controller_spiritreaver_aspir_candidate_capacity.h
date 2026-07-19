#pragma once

namespace automatoncontrollerspiritreaveraspircandidate
{
inline auto CanSelectCandidate(uint8 automatonMPP, uint32 targetMP) -> bool
{
    return automatonMPP < 75 && targetMP > 0;
}
} // namespace automatoncontrollerspiritreaveraspircandidate
