#pragma once

#include "common/cbasetypes.h"

namespace rangedoutcomehelpers
{

constexpr uint16 MsgShadowAbsorb           = 31;
constexpr uint16 MsgRangedAttackAbsorbs    = 382;
constexpr uint16 MsgAddEffectDamage        = 163;
constexpr uint16 MsgAddEffectRecoversHP    = 384;

struct MessageParam
{
    uint16 message;
    int32  param;

    constexpr auto operator==(const MessageParam&) const -> bool = default;
};

// Damage returned as a negative action parameter represents absorbed HP.
inline auto NormalizeRangedDamage(const uint16 message, const int32 param) -> MessageParam
{
    if (param < 0)
    {
        return { MsgRangedAttackAbsorbs, -param };
    }
    return { message, param };
}

// Only negative AddEffectDamage values are rewritten as HP recovery.
inline auto NormalizeRangedAdditionalEffect(const uint16 message, const int32 param) -> MessageParam
{
    if (message == MsgAddEffectDamage && param < 0)
    {
        return { MsgAddEffectRecoversHP, -param };
    }
    return { message, param };
}

inline auto ShouldUseShadowAbsorbOutcome(const bool hitOccurred, const uint8 shadowsTaken) -> bool
{
    return !hitOccurred && shadowsTaken > 0;
}

} // namespace rangedoutcomehelpers
