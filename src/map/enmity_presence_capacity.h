#pragma once

#include "common/cbasetypes.h"

namespace enmitypresencehelpers
{

inline auto IsTargetedByLiveOpponent(
    const bool alive,
    const uint16 mobBattleTargetID,
    const uint16 entityTargetID,
    const uint8 mobAllegiance,
    const uint8 entityAllegiance) -> bool
{
    return alive && mobBattleTargetID == entityTargetID && mobAllegiance != entityAllegiance;
}

template <typename ScanZone>
inline auto Resolve(const bool hasOwnEnmity, ScanZone&& scanZone) -> bool
{
    return hasOwnEnmity || scanZone();
}

} // namespace enmitypresencehelpers
