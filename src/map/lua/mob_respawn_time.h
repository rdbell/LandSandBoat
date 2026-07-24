#pragma once

#include <common/cbasetypes.h>

namespace mobrespawntime
{

struct Plan
{
    bool disable;
    bool registerPending;
};

inline auto plan(const uint32 seconds, const bool spawned, const bool hasZone) -> Plan
{
    return { seconds == 0, seconds != 0 && !spawned && hasZone };
}

} // namespace mobrespawntime
