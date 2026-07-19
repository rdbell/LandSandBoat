#pragma once

namespace mobcontrollerroamcallforhelp
{
// ShouldClear reports whether a roam action clears an active Call for Help flag.
constexpr auto ShouldClear(const bool callForHelpActive) -> bool
{
    return callForHelpActive;
}
} // namespace mobcontrollerroamcallforhelp
