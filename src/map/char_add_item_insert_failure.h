#pragma once
#include "common/cbasetypes.h"
namespace additeminsertfailurehelpers { struct Plan { bool reject{}; bool logFull{}; uint8 returnSlot{}; auto operator==(const Plan&) const -> bool = default; }; constexpr auto BuildPlan(uint8 slot) -> Plan { return slot == 0xFF ? Plan{ true, true, slot } : Plan{}; } }
