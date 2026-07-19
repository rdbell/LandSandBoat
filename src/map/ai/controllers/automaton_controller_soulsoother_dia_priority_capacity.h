#pragma once

namespace automatoncontrollersoulsootherdia
{
inline auto CanPrioritize(bool bioMissing, uint8 lightManeuvers) -> bool
{
    return bioMissing && lightManeuvers > 0;
}
} // namespace automatoncontrollersoulsootherdia
