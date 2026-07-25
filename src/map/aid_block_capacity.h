#pragma once

// Pure IsAidBlocked policy from charutils.

namespace aidblockhelpers
{
// IsAidBlocked mirrors target blocking aid unless the initiator is allied.
constexpr auto IsAidBlocked(const bool targetBlocksAid, const bool initiatorInAlliance) -> bool
{
    return targetBlocksAid && !initiatorInAlliance;
}
} // namespace aidblockhelpers
