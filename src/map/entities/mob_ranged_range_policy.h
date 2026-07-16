#pragma once
#include "common/cbasetypes.h"
namespace mobrangedrangehelpers { inline auto GetRangedAttackRange(int16 modifier) -> float { return modifier > 0 ? static_cast<float>(modifier) : 14.0F; } }
