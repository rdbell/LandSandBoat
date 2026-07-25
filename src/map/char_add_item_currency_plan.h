#pragma once
#include "common/cbasetypes.h"
namespace additemcurrencyhelpers { struct Plan { bool updateCurrency{}; uint8 slot{}; uint8 returnSlot{}; auto operator==(const Plan&) const -> bool = default; }; constexpr auto BuildPlan(bool currency) -> Plan { return currency ? Plan{ true, 0, 0 } : Plan{}; } }
