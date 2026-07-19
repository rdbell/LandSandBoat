#pragma once

namespace automatoncontrollerenfeebleadmission
{
inline auto CanUseEnfeeble(bool hasEnfeebleStatus, bool hasImmunity) -> bool
{
    return !hasEnfeebleStatus && !hasImmunity;
}
} // namespace automatoncontrollerenfeebleadmission
