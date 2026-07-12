#pragma once

#include <algorithm>
#include <cstdint>

// Pure xi.automaton.handleAttuner / getManeuverCount.
// Parity: internal/automaton AttunerDefIgnore / ManeuverCount

namespace attunerhelpers
{

constexpr std::uint16_t ItemAttunerAttachment = 8453;
constexpr std::uint16_t AttachmentIDOffset    = 0x2100;

// Attuner attachment slot id for CAutomatonEntity::hasAttachment.
constexpr auto AttunerAttachmentSlot() -> std::uint8_t
{
    return static_cast<std::uint8_t>(ItemAttunerAttachment - AttachmentIDOffset);
}

constexpr double kAttunerDefIgnore[4] = { 0.05, 0.10, 0.15, 0.20 };

constexpr auto ManeuverCount(int rawCount, const bool hasOverdrive) -> int
{
    if (rawCount < 0)
    {
        rawCount = 0;
    }
    int active = rawCount;
    if (active > 3)
    {
        active = 3;
    }
    if (active > 0 && hasOverdrive)
    {
        return 3;
    }
    return active;
}

// Pure product after host gates isAutomaton/hasAttuner/hasMaster/levels.
inline auto AttunerDefIgnore(const bool isAutomaton, const bool hasAttuner, const bool hasMaster, const std::uint8_t actorLevel, const std::uint8_t targetLevel, const int fireManeuverCount, const bool masterHasOverdrive) -> double
{
    if (!isAutomaton || !hasAttuner || !hasMaster)
    {
        return 0.0;
    }
    if (actorLevel >= targetLevel)
    {
        return 0.0;
    }
    int count = ManeuverCount(fireManeuverCount, masterHasOverdrive);
    if (count < 0)
    {
        count = 0;
    }
    if (count > 3)
    {
        count = 3;
    }
    return kAttunerDefIgnore[count];
}

} // namespace attunerhelpers
