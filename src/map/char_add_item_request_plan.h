#pragma once
#include "common/cbasetypes.h"
namespace additemrequesthelpers { enum class Decision : uint8 { Reject, MissingItem, SetQuantityAndDelegate }; constexpr auto BuildPlan(uint8 freeSlots, uint32 quantity, bool spawned) -> Decision { if (freeSlots == 0 || quantity == 0) return Decision::Reject; return spawned ? Decision::SetQuantityAndDelegate : Decision::MissingItem; } }
