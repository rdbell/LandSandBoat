#pragma once
namespace targetfindallegiancehelpers
{
// ShouldRejectInitialTargetAllegiance mirrors the non-AnyAllegiance branch.
constexpr auto ShouldRejectInitialTargetAllegiance(const bool hasInitialTarget, const bool allegianceMismatch) -> bool
{
    return hasInitialTarget && allegianceMismatch;
}
} // namespace targetfindallegiancehelpers
