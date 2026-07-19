#pragma once

#include <cstdint>

namespace mobcontrollermobskilloverride
{
// Resolve selects the script override only when it is a positive skill ID.
constexpr auto Resolve(const std::uint16_t selectedSkillID, const std::uint16_t overrideSkillID) -> std::uint16_t
{
    return overrideSkillID > 0 ? overrideSkillID : selectedSkillID;
}
} // namespace mobcontrollermobskilloverride
