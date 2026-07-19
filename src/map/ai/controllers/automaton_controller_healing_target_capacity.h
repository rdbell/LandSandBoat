#pragma once

#include <cstdint>

namespace automatoncontrollerhealingtarget
{
enum class Target : uint8_t
{
    None,
    Automaton,
    Master,
};

inline auto Select(bool hasHate, uint8_t automatonHPP, uint8_t masterHPP, float threshold, float masterDistance) -> Target
{
    if (hasHate)
    {
        if (automatonHPP <= 50)
        {
            return Target::Automaton;
        }
        if (masterHPP <= threshold && masterDistance < 20.0f)
        {
            return Target::Master;
        }
        return Target::None;
    }

    if (masterHPP <= threshold)
    {
        return Target::Master;
    }
    if (automatonHPP <= 50)
    {
        return Target::Automaton;
    }
    return Target::None;
}
} // namespace automatoncontrollerhealingtarget
