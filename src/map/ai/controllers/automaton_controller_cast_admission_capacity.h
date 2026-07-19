#pragma once

namespace automatoncontrollercastadmission
{
inline auto CanUseCast(bool spellUsable, bool hasRecast) -> bool
{
    return spellUsable && !hasRecast;
}
} // namespace automatoncontrollercastadmission
