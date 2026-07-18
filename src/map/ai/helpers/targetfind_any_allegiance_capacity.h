#pragma once
namespace targetfindanyallegiancehelpers
{
// ShouldRejectAnyAllegianceSelf mirrors validEntity's broad-target self exclusion.
constexpr auto ShouldRejectAnyAllegianceSelf(const bool targetAnyAllegiance, const bool isCaster) -> bool
{
    return targetAnyAllegiance && isCaster;
}
} // namespace targetfindanyallegiancehelpers
