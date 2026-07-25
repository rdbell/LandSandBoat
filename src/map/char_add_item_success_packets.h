#pragma once
#include "common/cbasetypes.h"
#include <array>
namespace additemsuccesspackethelpers { enum class Action : uint8 { Attribute, Same }; constexpr auto BuildPlan() -> std::array<Action, 2> { return { Action::Attribute, Action::Same }; } }
