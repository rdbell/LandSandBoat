#pragma once

namespace mobcontrollerroamengagegate
{
inline auto ShouldEngageFromEnmity(bool hasHighestEnmity, bool ignoreRoam) -> bool
{
    return hasHighestEnmity && !ignoreRoam;
}
} // namespace mobcontrollerroamengagegate
