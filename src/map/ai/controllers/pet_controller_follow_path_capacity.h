#pragma once

namespace petcontrollerfollowpath
{
inline auto ShouldRecalculate(bool following, float destinationDistance) -> bool
{
    return !following || destinationDistance > 2.0f;
}
} // namespace petcontrollerfollowpath
