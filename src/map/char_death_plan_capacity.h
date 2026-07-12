#pragma once

#include <algorithm>
#include <cstdint>

namespace chardeathplanhelpers
{

enum class Message : std::uint8_t
{
    FallsToGround,
    DefeatedBy,
};

struct Input
{
    bool         lastAttackerMatches{};
    bool         hasPet{};
    bool         mijinGakure{};
    bool         hasBattlefield{};
    bool         battlefieldLosesEXP{};
    std::uint8_t mainLevel{};
    std::uint8_t expLossLevel{};
    std::uint8_t expRetain{};
    std::int16_t experienceRetainedMod{};
};

struct Plan
{
    Message message{};
    bool    despawnPet{};
    bool    loseEXP{};
    float   retainPercent{};
};

inline auto Build(const Input& input) -> Plan
{
    Plan plan{
        .message    = input.lastAttackerMatches ? Message::DefeatedBy : Message::FallsToGround,
        .despawnPet = input.hasPet,
    };
    plan.loseEXP = !input.mijinGakure && (!input.hasBattlefield || input.battlefieldLosesEXP) && input.mainLevel >= input.expLossLevel;
    if (plan.loseEXP)
    {
        plan.retainPercent = std::clamp(input.expRetain + input.experienceRetainedMod / 100.0F, 0.0F, 1.0F);
    }
    return plan;
}

} // namespace chardeathplanhelpers
