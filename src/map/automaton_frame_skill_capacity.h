#pragma once

#include <cstdint>

// Pure LoadAutomatonStats frame → GetMaxSkill rank pins for EVA / DEF.
// Parity: internal/automaton frame_skill.go (slice 1612).

namespace automatonframeskillhelpers
{

// AutomatonFrame enum pins (enums/automaton.h).
constexpr std::uint8_t FrameHarlequin = 0x20;
constexpr std::uint8_t FrameValoredge = 0x21;
constexpr std::uint8_t FrameSharpshot = 0x22;
constexpr std::uint8_t FrameStormwaker = 0x23;

// GetMaxSkill rank for WorkingSkills.evasion by frame.
constexpr auto EvasionSkillRank(const std::uint8_t frame) -> std::uint8_t
{
    switch (frame)
    {
        case FrameValoredge:
            return 7;
        case FrameSharpshot:
            return 2;
        case FrameStormwaker:
            return 10;
        case FrameHarlequin:
        default:
            return 4;
    }
}

// GetMaxSkill rank for Mod::DEF by frame.
constexpr auto DefenseSkillRank(const std::uint8_t frame) -> std::uint8_t
{
    switch (frame)
    {
        case FrameValoredge:
            return 8;
        case FrameSharpshot:
        case FrameStormwaker:
            return 12;
        case FrameHarlequin:
        default:
            return 11;
    }
}

// Spell interrupt resistance pin for all frames.
constexpr std::int16_t SpellInterrupt = 85;

} // namespace automatonframeskillhelpers
