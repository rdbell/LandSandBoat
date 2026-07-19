#pragma once

namespace automatoncontrollerstandback
{
inline auto ShouldStandBack(bool hasMaster, bool animatorII, bool isValoredge, bool rangedHead) -> bool
{
    if (hasMaster)
    {
        return animatorII;
    }
    if (isValoredge)
    {
        return false;
    }
    return rangedHead;
}
} // namespace automatoncontrollerstandback
