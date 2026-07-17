#pragma once

#include <common/cbasetypes.h>

// Pure loginHelpers::isZoneAtPlayerCap composition helpers (slice 2841).
//
// Production host: loginHelpers::isZoneAtPlayerCap in login_helpers.cpp.
// Helpers take host-injected settings + SQL zonetype/pop only (no DB/settings).

namespace loginHelpers
{

// ZoneTypeInstanced is the ZONE_TYPE::INSTANCED bit (0x0100) checked by
// isZoneAtPlayerCap after the zone_settings.zonetype column is injected.
constexpr uint16 ZoneTypeInstanced = 0x0100;

// IsZoneTypeInstanced reports whether zoneType has the INSTANCED bit set.
inline auto IsZoneTypeInstanced(const uint16 zoneType) -> bool
{
    return (zoneType & ZoneTypeInstanced) != 0;
}

// ZonePlayerCapThreshold returns the effective population threshold after
// cap and reserved settings are resolved:
//   threshold = isGM ? cap : (cap > reserved ? cap - reserved : 0)
// Cap==0 is handled by IsZoneAtPlayerCap (disabled), not here.
inline auto ZonePlayerCapThreshold(const uint16 cap, const uint16 reserved, const bool isGM) -> uint16
{
    if (isGM)
    {
        return cap;
    }
    if (cap > reserved)
    {
        return static_cast<uint16>(cap - reserved);
    }
    return 0;
}

// IsZoneAtPlayerCap is the pure half of loginHelpers::isZoneAtPlayerCap once
// settings and the zone pop / zonetype query are injected:
//   1. cap == 0 → false (cap disabled)
//   2. instanced → false (host sets instanced from IsZoneTypeInstanced(zonetype))
//   3. pop >= ZonePlayerCapThreshold(cap, reserved, isGM)
// Missing zone rows and DB failures remain host-side (production returns false
// when the prepared statement yields no row).
inline auto IsZoneAtPlayerCap(
    const uint16 cap,
    const uint16 reserved,
    const bool   isGM,
    const uint32 pop,
    const bool   instanced) -> bool
{
    if (cap == 0)
    {
        return false;
    }
    if (instanced)
    {
        return false;
    }
    return pop >= static_cast<uint32>(ZonePlayerCapThreshold(cap, reserved, isGM));
}

} // namespace loginHelpers
