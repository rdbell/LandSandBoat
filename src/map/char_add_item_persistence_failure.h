#pragma once
#include "common/cbasetypes.h"
namespace additempersistencefailurehelpers { struct Plan { bool reject{}; bool removeInserted{}; uint8 returnSlot{}; auto operator==(const Plan&) const -> bool = default; }; constexpr auto BuildPlan(bool persisted) -> Plan { return persisted ? Plan{} : Plan{ true, true, 0xFF }; } }
