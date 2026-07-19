#pragma once

namespace automatoncontrollerlowhpelementalpriority
{
inline auto ShouldPrioritizeElementalForLowHP(int hpp, int hp) -> bool
{
    return hpp <= 30 && hp <= 300;
}
} // namespace automatoncontrollerlowhpelementalpriority
