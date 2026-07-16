#pragma once
#include "common/cbasetypes.h"
namespace mobroamratehelpers { inline auto GetRoamRate(int16 modifier) -> float { return static_cast<float>(modifier) / 10.0F; } }
