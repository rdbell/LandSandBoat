#pragma once

namespace mobcontrollerspecialskilltarget
{
inline auto CanSelectTarget(bool selfValid, bool hasTarget, bool withinRange) -> bool
{
    return selfValid || (hasTarget && withinRange);
}
} // namespace mobcontrollerspecialskilltarget
