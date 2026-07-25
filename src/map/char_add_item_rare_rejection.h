#pragma once
#include "common/cbasetypes.h"
namespace additemrarerejectionhelpers { struct Plan { bool reject{}; bool sendItemEx{}; uint8 returnSlot{}; auto operator==(const Plan&) const -> bool = default; }; constexpr auto BuildPlan(bool rare, bool alreadyHas, bool silence) -> Plan { return rare && alreadyHas ? Plan{ true, !silence, 0xFF } : Plan{}; } }
