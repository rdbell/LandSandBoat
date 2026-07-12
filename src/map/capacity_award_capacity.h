#pragma once

#include <cstdint>

// Pure AddCapacityPoints award policy from charutils.
// Chain message helpers already live in capacitydistributehelpers.

namespace capacityawardhelpers
{

// ShouldRejectDead mirrors isDead early return.
constexpr auto ShouldRejectDead(const bool isDead) -> bool
{
    return isDead;
}

// ApplyExpRate multiplies capacity by map.EXP_RATE (uint32 cast).
inline auto ApplyExpRate(const std::uint32_t capacityPoints, const float expRate) -> std::uint32_t
{
    return static_cast<std::uint32_t>(static_cast<float>(capacityPoints) * expRate);
}

// ShouldAwardCapacityPoints mirrors capacityPoints > 0 after rate.
constexpr auto ShouldAwardCapacityPoints(const std::uint32_t capacityPoints) -> bool
{
    return capacityPoints > 0;
}

// CapacityAwardMessage classifies battle-message2 capacity award types.
enum class CapacityAwardMessage : std::uint8_t
{
    CapacityPointsGained = 0,
    CapacityChain        = 1,
};

// SelectCapacityAwardMessage mirrors chain path with non-zero chain number.
constexpr auto SelectCapacityAwardMessage(const bool sendChainMessage, const bool chainNumberNonZero) -> CapacityAwardMessage
{
    if (sendChainMessage && chainNumberNonZero)
    {
        return CapacityAwardMessage::CapacityChain;
    }
    return CapacityAwardMessage::CapacityPointsGained;
}

// ShouldAdvanceChainNumber mirrors sendChainMessage path that increments chain.
constexpr auto ShouldAdvanceChainNumber(const bool sendChainMessage) -> bool
{
    return sendChainMessage;
}

// ShouldFireRoeCapacity mirrors PMob != PChar (only mob kills).
constexpr auto ShouldFireRoeCapacity(const bool mobIsSelf) -> bool
{
    return !mobIsSelf;
}

} // namespace capacityawardhelpers
