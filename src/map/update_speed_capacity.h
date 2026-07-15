#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace updatespeedhelpers
{

// Weight/flee clamp bounds from UpdateSpeed.
constexpr float WeightFactorMin = 0.1f;
constexpr float WeightFactorMax = 1.0f;
constexpr float FleeFactorMin   = 1.0f;
constexpr float FleeFactorMax   = 2.0f;
constexpr float GearFactorMin   = 1.0f;
constexpr float GearFactorMax   = 1.25f;

// Mazurka+Quickening shared additive cap.
constexpr uint8 MazurkaQuickeningCap = 10;

// Mob run multiplier weight-penalty cut.
constexpr float MobRunWeightPenaltyScale = 0.48f;

// UpdateSpeedParams injects host mods/settings into the pure formula.
struct UpdateSpeedParams
{
    bool  isMounted{ false };
    uint8 mountSpeedSetting{ 0 }; // map.MOUNT_SPEED before /2
    int16 mountMoveMod{ 0 };      // Mod::MOUNT_MOVE percent

    uint8 baseSpeed{ 0 };
    int16 moveSpeedOverride{ 0 }; // Mod::MOVE_SPEED_OVERRIDE

    int8  stackableMod{ 0 };      // MOVE_SPEED_STACKABLE as int8
    int16 weightPenaltyMod{ 0 };  // MOVE_SPEED_WEIGHT_PENALTY
    int16 fleeMod{ 0 };           // MOVE_SPEED_FLEE (1/10000)
    int16 cheerMod{ 0 };          // MOVE_SPEED_CHEER
    uint8 boltersRoll{ 0 };       // MOVE_SPEED_BOLTERS_ROLL
    bool  isPC{ false };
    int16 gearBonusMod{ 0 };      // getMaxGearMod(MOVE_SPEED_GEAR_BONUS) for PC
    int16 quickeningMod{ 0 };
    int16 mazurkaMod{ 0 };
    uint8 speedLimit{ 80 };       // map.SPEED_LIMIT

    bool  run{ false };
    bool  isMob{ false };
    float mobRunMultiplier{ 1.0f }; // map.MOB_RUN_SPEED_MULTIPLIER
    int16 mobRunSpeedMultMod{ 0 };  // MOBMOD_RUN_SPEED_MULT (0 = unset)
    int16 mobWeightPenaltyMod{ 0 }; // mob's MOVE_SPEED_WEIGHT_PENALTY
};

// ClampFloat mirrors std::clamp for float factors.
inline auto ClampFloat(const float v, const float lo, const float hi) -> float
{
    return std::clamp(v, lo, hi);
}

// ResolveMountSpeed mirrors the isMounted branch.
inline auto ResolveMountSpeed(const uint8 mountSpeedSetting, const int16 mountMoveMod) -> int16
{
    int16 output = static_cast<int16>(mountSpeedSetting / 2);
    output       = static_cast<int16>(output * (1.0f + static_cast<float>(mountMoveMod) / 100.0f));
    return output;
}

// ResolveRunMultiplier mirrors the mob run multiplier block.
// Returns 1.0 when run path does not apply a boost.
inline auto ResolveRunMultiplier(
    const bool run,
    const int16 outputSpeedBeforeRun,
    const bool isMob,
    const float mobRunMultiplierSetting,
    const int16 mobRunSpeedMultMod,
    const int16 mobWeightPenaltyMod) -> float
{
    if (!run || outputSpeedBeforeRun <= 0)
    {
        return 1.0f;
    }
    float multiplier = mobRunMultiplierSetting;
    if (multiplier <= 1.0f || !isMob)
    {
        return 1.0f;
    }
    if (mobRunSpeedMultMod > 0)
    {
        multiplier = static_cast<float>(mobRunSpeedMultMod) / 100.0f;
    }
    if (mobWeightPenaltyMod > 0)
    {
        multiplier *= MobRunWeightPenaltyScale;
    }
    return std::max(multiplier, 1.0f);
}

// ResolveUpdateSpeed is the full pure UpdateSpeed formula (before store to speed).
// Returns the clamped uint8 speed value.
inline auto ResolveUpdateSpeed(const UpdateSpeedParams& p) -> uint8
{
    int16 outputSpeed = 0;

    if (p.isMounted)
    {
        outputSpeed = ResolveMountSpeed(p.mountSpeedSetting, p.mountMoveMod);
    }
    else if (p.baseSpeed == 0 || p.moveSpeedOverride < 0)
    {
        outputSpeed = 0;
    }
    else if (p.moveSpeedOverride > 0)
    {
        outputSpeed = p.moveSpeedOverride;
    }
    else
    {
        const float weightFactor = ClampFloat(
            1.0f - static_cast<float>(p.weightPenaltyMod) / 100.0f,
            WeightFactorMin,
            WeightFactorMax);
        const float fleeFactor = ClampFloat(
            1.0f + static_cast<float>(p.fleeMod) / 10000.0f,
            FleeFactorMin,
            FleeFactorMax);
        const float cheerFactor = (99.0f + static_cast<float>(p.cheerMod)) / 99.0f;

        float gearFactor = 1.0f;
        if (p.isPC)
        {
            gearFactor = ClampFloat(
                1.0f + static_cast<float>(p.gearBonusMod) / 100.0f,
                GearFactorMin,
                GearFactorMax);
        }

        const uint8 mazurkaQuickening = static_cast<uint8>(std::clamp<int16>(
            static_cast<int16>(p.quickeningMod + p.mazurkaMod),
            0,
            static_cast<int16>(MazurkaQuickeningCap)));

        // MUST BE DONE IN THIS ORDER (LSB comment); int16 intermediate floors via float cast.
        outputSpeed = static_cast<int16>(p.baseSpeed) + p.stackableMod;
        outputSpeed = static_cast<int16>(outputSpeed * weightFactor);
        outputSpeed = static_cast<int16>(outputSpeed * fleeFactor);
        outputSpeed = static_cast<int16>(outputSpeed * cheerFactor);
        outputSpeed = static_cast<int16>(outputSpeed + p.boltersRoll);
        outputSpeed = static_cast<int16>(outputSpeed * gearFactor);
        if (outputSpeed > 0)
        {
            outputSpeed = static_cast<int16>(outputSpeed + mazurkaQuickening);
        }

        if (p.isPC)
        {
            outputSpeed = std::clamp<int16>(outputSpeed, 0, static_cast<int16>(p.speedLimit));
        }

        const float runMult = ResolveRunMultiplier(
            p.run,
            outputSpeed,
            p.isMob,
            p.mobRunMultiplier,
            p.mobRunSpeedMultMod,
            p.mobWeightPenaltyMod);
        if (runMult != 1.0f)
        {
            outputSpeed = static_cast<int16>(outputSpeed * runMult);
        }
    }

    return static_cast<uint8>(std::clamp<int16>(
        outputSpeed,
        std::numeric_limits<uint8>::min(),
        std::numeric_limits<uint8>::max()));
}

} // namespace updatespeedhelpers
