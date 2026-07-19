#pragma once

#include <cstdint>

namespace automatoncontrollertpskillselectionresult
{
inline auto HasSelectedTPSkill(int8 maneuvers) -> bool
{
    return maneuvers != -1;
}
} // namespace automatoncontrollertpskillselectionresult
