#pragma once

namespace mobcontrollerchasepathrefresh
{
// ShouldRefresh reports whether an active chase path no longer reaches its target.
constexpr auto ShouldRefresh(const bool destinationMatchesTarget) -> bool
{
    return !destinationMatchesTarget;
}
} // namespace mobcontrollerchasepathrefresh
