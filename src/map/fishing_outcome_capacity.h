#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure fishing outcome formulas from fishingutils (regen, delay, movement,
// lose/snap/break chances). Parity: internal/fishingutils (regen, delay_movement,
// lose_snap_break; slice 1617 production wire).

namespace fishingoutcomehelpers
{

// Rod item IDs (FISHINGROD in fishingutils.h).
constexpr std::uint16_t RodIDLuShang  = 17386;
constexpr std::uint16_t RodIDEbisu    = 17011;
constexpr std::uint16_t RodIDLuShang1 = 19320;
constexpr std::uint16_t RodIDEbisu1   = 19321;

// FISHINGSIZETYPE
constexpr std::uint8_t SizeTypeSmall = 0;
constexpr std::uint8_t SizeTypeLarge = 1;

// FISHINGCATCHTYPE
constexpr std::uint8_t CatchTypeNone      = 0;
constexpr std::uint8_t CatchTypeSmallFish = 1;
constexpr std::uint8_t CatchTypeBigFish   = 2;
constexpr std::uint8_t CatchTypeItem      = 3;
constexpr std::uint8_t CatchTypeMob       = 4;
constexpr std::uint8_t CatchTypeChest     = 5;

// FISHINGFAILTYPE
constexpr std::uint8_t FailTypeNone         = 0;
constexpr std::uint8_t FailTypeLineSnap     = 2;
constexpr std::uint8_t FailTypeRodBreak     = 3;
constexpr std::uint8_t FailTypeLostTooSmall = 7;
constexpr std::uint8_t FailTypeLostLowSkill = 8;
constexpr std::uint8_t FailTypeLostTooBig   = 9;

struct LsbRet
{
    std::uint8_t failReason = 0;
    std::uint8_t chance     = 0;
};

// Rod fields needed by outcome pure formulas (subset of rod_t).
struct RodView
{
    std::uint16_t rodID        = 0;
    std::uint8_t  sizeType     = 0;
    std::uint16_t minRank      = 0;
    std::uint16_t maxRank      = 0;
    std::uint8_t  smDelayBonus = 0;
    std::uint8_t  smMoveBonus  = 0;
    std::uint8_t  lgDelayBonus = 0;
    std::uint8_t  lgMoveBonus  = 0;
    bool          legendary    = false;
    bool          breakable    = false;
};

// Pure CalculateRegen: base 128, size/rod/skill/chest adjustments, clamp [0,182].
// Intermediate math uses int (matches Go pure); penalty path wraps through uint8
// before final clamp like LSB `regen += (1 + (uint8)floor(...))`.
inline auto CalculateRegen(const std::uint8_t fishingSkill, const std::uint16_t rodID, const std::uint8_t catchType, const std::uint8_t sizeType,
                           const std::uint8_t catchSkill, const bool legendaryCatch, const bool isNM) -> std::uint8_t
{
    auto              regen     = 128;
    constexpr auto    drainDiff = 12;
    constexpr auto    regenDiff = 24;
    std::uint8_t      regenMod  = 0;

    if (rodID == RodIDEbisu)
    {
        regenMod = 11;
    }

    if (sizeType > SizeTypeSmall && rodID != RodIDEbisu)
    {
        regen++;
    }

    if (rodID == RodIDLuShang || rodID == RodIDEbisu || rodID == RodIDLuShang1 || rodID == RodIDEbisu1)
    {
        if (legendaryCatch)
        {
            regen -= (rodID == RodIDLuShang || rodID == RodIDLuShang1) ? 1 : 2;
        }
        if (catchType == CatchTypeMob)
        {
            regen -= 3;
        }
    }

    if (catchType <= CatchTypeMob && !isNM)
    {
        if (static_cast<int>(catchSkill) <= static_cast<int>(fishingSkill) + static_cast<int>(regenMod) - drainDiff)
        {
            auto divMod = 1.5f;
            if (rodID == RodIDLuShang || rodID == RodIDLuShang1)
            {
                divMod = 1.4f;
            }
            if (rodID == RodIDEbisu || rodID == RodIDEbisu1)
            {
                divMod = 1.3f;
            }
            const auto diff = static_cast<float>(static_cast<int>(fishingSkill) + static_cast<int>(regenMod) - drainDiff - static_cast<int>(catchSkill)) / divMod;
            auto       sub  = static_cast<std::uint8_t>(1 + static_cast<std::uint8_t>(std::floor(diff)));
            if (static_cast<int>(sub) > regen)
            {
                sub = static_cast<std::uint8_t>(regen);
            }
            regen -= static_cast<int>(sub);
        }

        if (catchType < CatchTypeItem && static_cast<int>(catchSkill) - static_cast<int>(regenMod) >= static_cast<int>(fishingSkill) + regenDiff)
        {
            auto multMod = 0.5f;
            if (rodID == RodIDLuShang || rodID == RodIDLuShang1)
            {
                multMod = 0.45f;
            }
            if (rodID == RodIDEbisu || rodID == RodIDEbisu1)
            {
                multMod = 0.4f;
            }
            const auto diff = static_cast<float>(static_cast<int>(catchSkill) - static_cast<int>(regenMod) - (static_cast<int>(fishingSkill) + regenDiff)) * multMod;
            const auto add  = static_cast<std::uint8_t>(1 + static_cast<std::uint8_t>(std::floor(diff)));
            // uint8 wrap before final clamp (LSB).
            regen = static_cast<int>(static_cast<std::uint8_t>(regen) + add);
        }
    }

    if (catchType == CatchTypeChest)
    {
        if (fishingSkill > catchSkill)
        {
            regen -= static_cast<int>(fishingSkill - catchSkill) / 5;
        }
    }

    if (regen < 0)
    {
        return 0;
    }
    if (regen > 182)
    {
        return 182;
    }
    return static_cast<std::uint8_t>(regen);
}

// Pure CalculateDelay with penguin-ring inject (Mod::PENGUIN_RING_EFFECT > 0).
inline auto CalculateDelay(const std::uint8_t baseDelay, const std::uint8_t sizeType, const RodView& rod, const std::uint8_t count, const bool penguinRing)
    -> std::uint8_t
{
    const auto multiplier = 1.0f + (0.1f * (static_cast<float>(count) - 1.0f));
    auto       delay      = static_cast<std::uint8_t>(std::floor(static_cast<float>(baseDelay) * multiplier));

    if (sizeType == SizeTypeSmall)
    {
        delay = static_cast<std::uint8_t>(delay + rod.smDelayBonus);
    }
    else
    {
        delay = static_cast<std::uint8_t>(delay + rod.lgDelayBonus);
    }

    if (penguinRing)
    {
        delay = static_cast<std::uint8_t>(delay + 2);
    }

    return std::min<std::uint8_t>(15, delay);
}

// Pure CalculateMovement with penguin-ring inject.
inline auto CalculateMovement(const std::uint8_t baseMove, const std::uint8_t sizeType, const RodView& rod, const std::uint8_t count, const bool penguinRing)
    -> std::uint8_t
{
    const auto multiplier = 1.0f + (0.1f * (static_cast<float>(count) - 1.0f));
    auto       movement   = static_cast<std::uint8_t>(std::floor(static_cast<float>(baseMove) * multiplier));

    if (sizeType == SizeTypeSmall)
    {
        movement = static_cast<std::uint8_t>(movement + rod.smMoveBonus);
    }
    else
    {
        movement = static_cast<std::uint8_t>(movement + rod.lgMoveBonus);
    }

    if (penguinRing)
    {
        movement = static_cast<std::uint8_t>(movement + 2);
    }

    return std::min<std::uint8_t>(15, movement);
}

// Pure CalculateLoseChance. legendary is signature-parity only (unused in body).
inline auto CalculateLoseChance(const std::uint8_t catchType, const std::uint8_t fishingSkill, const std::uint8_t maxSkill, const std::uint8_t sizeType,
                                const bool /*legendary*/, const std::uint8_t ranking, const RodView& rod) -> LsbRet
{
    std::uint8_t tooBigChance   = 0;
    std::uint8_t tooSmallChance = 0;
    std::uint8_t lowSkillChance = 0;
    LsbRet       lsb{};

    if (!rod.legendary)
    {
        if (sizeType > rod.sizeType && static_cast<int>(ranking) > static_cast<int>(rod.maxRank))
        {
            tooBigChance = 50;
            if (fishingSkill < maxSkill)
            {
                tooBigChance = static_cast<std::uint8_t>(tooBigChance + (maxSkill - fishingSkill));
            }
            if (fishingSkill > maxSkill)
            {
                tooBigChance = static_cast<std::uint8_t>(tooBigChance - (fishingSkill - maxSkill));
            }
        }
        else if (sizeType < rod.sizeType && static_cast<int>(ranking) < static_cast<int>(rod.minRank))
        {
            tooSmallChance = 50;
            if (fishingSkill < maxSkill)
            {
                tooSmallChance = static_cast<std::uint8_t>(tooSmallChance + (maxSkill - fishingSkill));
            }
            if (fishingSkill > maxSkill)
            {
                const auto diff = static_cast<std::uint8_t>(fishingSkill - maxSkill);
                tooSmallChance  = static_cast<std::uint8_t>(tooSmallChance - std::min(diff, tooSmallChance));
            }
        }
    }

    if (catchType < CatchTypeItem && static_cast<int>(fishingSkill) + 7 < static_cast<int>(maxSkill))
    {
        const auto diff    = static_cast<std::uint8_t>(static_cast<int>(maxSkill) - (static_cast<int>(fishingSkill) + 7));
        const auto diffAdd = static_cast<float>(diff) * 0.8f;
        lowSkillChance     = static_cast<std::uint8_t>(std::floor(diffAdd));
    }

    if (tooBigChance > 0 && tooBigChance > lowSkillChance)
    {
        lsb.failReason = FailTypeLostTooBig;
        lsb.chance     = std::clamp<std::uint8_t>(tooBigChance, 0, 50);
    }
    else if (tooSmallChance > 0 && tooSmallChance > lowSkillChance)
    {
        lsb.failReason = FailTypeLostTooSmall;
        lsb.chance     = std::clamp<std::uint8_t>(tooSmallChance, 0, 50);
    }
    else if (catchType < CatchTypeItem && lowSkillChance > 0)
    {
        lsb.failReason = FailTypeLostLowSkill;
        lsb.chance     = std::clamp<std::uint8_t>(lowSkillChance, 0, 55);
    }

    return lsb;
}

// Pure CalculateSnapChance. catchType is signature-parity only.
inline auto CalculateSnapChance(const std::uint8_t /*catchType*/, const std::uint8_t fishingSkill, const std::uint8_t maxSkill, const std::uint8_t sizeType,
                                const bool legendary, const std::uint8_t ranking, const RodView& rod) -> LsbRet
{
    std::uint8_t levelDiffBonus = 0;
    std::uint8_t sizePenalty    = 0;
    std::uint8_t legendaryBonus = 0;
    LsbRet       lsb{};

    if (static_cast<int>(fishingSkill) + 10 > static_cast<int>(maxSkill))
    {
        levelDiffBonus = 2;
    }

    if (!rod.legendary && sizeType > rod.sizeType)
    {
        sizePenalty = 2;
    }

    if (legendary)
    {
        if (!rod.legendary)
        {
            sizePenalty = static_cast<std::uint8_t>(sizePenalty + 3);
        }
        else
        {
            legendaryBonus = 1;
        }
    }

    const auto totalDurability =
        static_cast<std::uint8_t>(static_cast<int>(rod.maxRank) + static_cast<int>(levelDiffBonus) + static_cast<int>(legendaryBonus) - static_cast<int>(sizePenalty));

    if (ranking > totalDurability)
    {
        const auto strDuraDiff = static_cast<std::uint8_t>(ranking - totalDurability);
        const auto chance      = static_cast<std::uint8_t>(std::floor(static_cast<float>(strDuraDiff) * 8.5f));
        lsb.failReason         = FailTypeLineSnap;
        lsb.chance             = std::clamp<std::uint8_t>(chance, 0, 55);
    }

    return lsb;
}

// Pure CalculateBreakChance. catchType is signature-parity only.
inline auto CalculateBreakChance(const std::uint8_t /*catchType*/, const std::uint8_t fishingSkill, const std::uint8_t maxSkill, const std::uint8_t sizeType,
                                 const bool legendary, const std::uint8_t ranking, const RodView& rod) -> LsbRet
{
    std::uint8_t levelDiffBonus = 0;
    std::uint8_t legendaryBonus = 0;
    std::uint8_t sizePenalty    = 0;
    LsbRet       lsb{};

    if (!rod.breakable)
    {
        return lsb;
    }

    if (static_cast<int>(fishingSkill) + 10 > static_cast<int>(maxSkill))
    {
        levelDiffBonus = 2;
    }

    if (!rod.legendary && sizeType > rod.sizeType)
    {
        sizePenalty = 2;
    }
    else if (rod.legendary && sizeType == SizeTypeLarge)
    {
        legendaryBonus = 1;
    }

    if (!rod.legendary && legendary)
    {
        sizePenalty = 5;
    }

    const auto threshold = static_cast<int>(rod.maxRank) + static_cast<int>(levelDiffBonus) + static_cast<int>(legendaryBonus);
    if (static_cast<int>(ranking) > threshold)
    {
        const auto strDuraDiff = static_cast<std::uint8_t>(static_cast<int>(ranking) - threshold);
        const auto sum         = static_cast<float>(static_cast<int>(strDuraDiff) + static_cast<int>(sizePenalty));
        const auto chance      = static_cast<std::uint8_t>(std::floor(sum * 1.3f));
        lsb.failReason         = FailTypeRodBreak;
        lsb.chance             = std::clamp<std::uint8_t>(chance, 0, 55);
    }

    return lsb;
}

// Host helper: project rod_t-compatible fields into RodView.
inline auto MakeRodView(const std::uint16_t rodID, const std::uint8_t sizeType, const std::uint16_t minRank, const std::uint16_t maxRank,
                        const std::uint8_t smDelayBonus, const std::uint8_t smMoveBonus, const std::uint8_t lgDelayBonus, const std::uint8_t lgMoveBonus,
                        const bool legendary, const bool breakable) -> RodView
{
    RodView view;
    view.rodID        = rodID;
    view.sizeType     = sizeType;
    view.minRank      = minRank;
    view.maxRank      = maxRank;
    view.smDelayBonus = smDelayBonus;
    view.smMoveBonus  = smMoveBonus;
    view.lgDelayBonus = lgDelayBonus;
    view.lgMoveBonus  = lgMoveBonus;
    view.legendary    = legendary;
    view.breakable    = breakable;
    return view;
}

} // namespace fishingoutcomehelpers
