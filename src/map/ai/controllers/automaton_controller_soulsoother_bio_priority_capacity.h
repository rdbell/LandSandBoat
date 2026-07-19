#pragma once

namespace automatoncontrollersoulsootherbio
{
inline auto CanPrioritize(bool diaMissing, uint8 darkManeuvers) -> bool
{
    return diaMissing && darkManeuvers > 0;
}
} // namespace automatoncontrollersoulsootherbio
