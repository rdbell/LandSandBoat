#pragma once

namespace mobcontrollerpartylinkengagement
{
// CanEngage reports whether an eligible party member joins the current fight.
template <typename CanLink>
constexpr auto CanEngage(const bool isRoaming, CanLink&& canLink) -> bool
{
    return isRoaming && canLink();
}
} // namespace mobcontrollerpartylinkengagement
