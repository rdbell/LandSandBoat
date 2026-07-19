#pragma once

#include "common/cbasetypes.h"

namespace petcontrollerimmobile
{
inline auto IsImmobile(uint16 petID) -> bool
{
    return petID == 75 || petID == 17 || petID == 18 || petID == 19;
}
} // namespace petcontrollerimmobile
