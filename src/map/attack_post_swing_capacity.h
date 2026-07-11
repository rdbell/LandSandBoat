#pragma once

#include "common/cbasetypes.h"

namespace attackpostswinghelpers
{

constexpr uint8  ResolutionMiss  = 1;
constexpr uint8  ResolutionParry = 3;
constexpr uint8  AttackTypeDaken = 9;
constexpr uint16 MsgShadowAbsorb = 31;

inline auto NormalizeMissParam(const uint8 resolution, const uint16 message, const int32 param) -> int32
{
    if (resolution == ResolutionMiss && message != MsgShadowAbsorb)
    {
        return 0;
    }
    return param;
}

inline auto ShouldRunEnspellAndSpikes(const uint8 resolution, const uint8 attackType) -> bool
{
    return resolution != ResolutionMiss && resolution != ResolutionParry && attackType != AttackTypeDaken;
}

inline auto ShouldRunEnspell(const uint8 targetHPP) -> bool
{
    return targetHPP > 0;
}

template <typename HasBattuta>
inline auto ShouldRunParrySpikes(const uint8 resolution, HasBattuta&& hasBattuta) -> bool
{
    return resolution == ResolutionParry && hasBattuta();
}

} // namespace attackpostswinghelpers
