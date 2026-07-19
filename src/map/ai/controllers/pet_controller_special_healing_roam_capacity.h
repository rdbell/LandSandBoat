#pragma once

namespace petcontrollerspecialhealingroam
{
inline auto ShouldHold(bool isWyvern, bool isAutomaton, bool isHealing) -> bool
{
    return (isWyvern || isAutomaton) && isHealing;
}
} // namespace petcontrollerspecialhealingroam
