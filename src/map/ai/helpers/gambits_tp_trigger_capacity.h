#pragma once

#include <cstdint>

namespace gambitstptrigger
{
enum class Trigger : uint16_t
{
    ASAP,
    Random,
    Opener,
    Closer,
    CloserUntilTP,
};

struct Result
{
    bool     ready;
    uint16_t value;
};

constexpr auto Evaluate(
    const bool hasTarget, const uint16_t tp, const Trigger trigger, uint16_t value, const uint16_t randomRoll,
    const bool partyHasTP, const bool skillchainOld, const bool skillchainTierZero) -> Result
{
    if (!hasTarget)
    {
        return { false, value };
    }
    if (tp >= 3000)
    {
        return { true, value };
    }
    const bool skillchain = skillchainOld && skillchainTierZero;
    switch (trigger)
    {
        case Trigger::ASAP:
            return { true, value };
        case Trigger::Random:
            value = value <= 1000 ? 1000 : value;
            return { tp >= value && randomRoll < value, value };
        case Trigger::Opener:
            value = value <= 1000 ? 1000 : value;
            return { partyHasTP, value };
        case Trigger::Closer:
            return { skillchain, value };
        case Trigger::CloserUntilTP:
            value = value <= 1500 ? 1500 : value;
            return { tp >= value || skillchain, value };
    }
    return { false, value };
}
} // namespace gambitstptrigger
