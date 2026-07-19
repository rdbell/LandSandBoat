#pragma once

namespace automatoncontrollermobskilladmission
{
inline auto CanUseMobSkill(bool hasRecast) -> bool
{
    return !hasRecast;
}
} // namespace automatoncontrollermobskilladmission
