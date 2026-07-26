#pragma once

#include "zone.h"

namespace synthfailurehelpers
{

constexpr auto ShouldBroadcastFailure(const uint16 zoneID) -> bool
{
    return zoneID != ZONE_MONORAIL_PRE_RELEASE && zoneID != ZONE_49 && zoneID < MAX_ZONEID;
}

} // namespace synthfailurehelpers
