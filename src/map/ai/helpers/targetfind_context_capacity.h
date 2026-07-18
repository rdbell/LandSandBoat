#pragma once

namespace targetfindcontexthelpers
{

// ShouldRejectContext mirrors validEntity's shared-context gate. The host
// injects comparison results; ignoring battle ID suppresses only that final
// mismatch, not confrontation, battlefield, or instance mismatches.
inline auto ShouldRejectContext(const bool confrontationMismatch,
                                const bool battlefieldMismatch,
                                const bool instanceMismatch,
                                const bool checkBattleID,
                                const bool battleIDMismatch) -> bool
{
    return confrontationMismatch || battlefieldMismatch || instanceMismatch || (checkBattleID && battleIDMismatch);
}

} // namespace targetfindcontexthelpers
