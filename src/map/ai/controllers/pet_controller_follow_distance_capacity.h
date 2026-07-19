#pragma once

namespace petcontrollerfollowdistance
{
inline auto ShouldHold(float distance) -> bool
{
    return distance <= 2.1f;
}
} // namespace petcontrollerfollowdistance
