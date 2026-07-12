#pragma once

#include <cmath>
#include <cstdint>

// Pure xi.combat.tp.calculateTPReturn / battleutils::CalculateBaseTP delay→TP.
// Parity: internal/attackutils CalculateTPReturn.

namespace tpreturnhelpers
{

// UsePCOrPetFormula: non-mob gainee, or charmed mob under PC master.
// Mirrors Lua: (gainee and objType ~= MOB) or isCharmedPCPet
constexpr auto UsePCOrPetTPFormula(const bool isMob, const bool isCharmedPCPet) -> bool
{
    return !isMob || isCharmedPCPet;
}

// IsCharmedPCPet inject: TYPE_MOB && isCharmed && master is PC.
constexpr auto IsCharmedPCPet(const bool isMob, const bool isCharmed, const bool hasPCMaster) -> bool
{
    return isMob && isCharmed && hasPCMaster;
}

inline auto PCOrPetTPReturn(const std::int32_t delay) -> double
{
    const auto d = static_cast<double>(delay);
    if (delay > 900)
    {
        return 173.0 + (d - 900.0) * 28.0 / 360.0;
    }
    if (delay > 720)
    {
        return 161.0 + (d - 720.0) * 24.0 / 360.0;
    }
    if (delay > 630)
    {
        return 154.0 + (d - 630.0) * 28.0 / 360.0;
    }
    if (delay > 540)
    {
        return 149.0 + (d - 540.0) * 20.0 / 360.0;
    }
    if (delay > 180)
    {
        return 61.0 + (d - 180.0) * 88.0 / 360.0;
    }
    return 61.0 + (d - 180.0) * 63.0 / 360.0;
}

inline auto MobTPReturn(const std::int32_t delay) -> double
{
    const auto d = static_cast<double>(delay);
    if (delay > 530)
    {
        return 145.0 + (d - 530.0) * 35.0 / 470.0;
    }
    if (delay > 480)
    {
        return 130.0 + (d - 480.0) * 15.0 / 30.0;
    }
    if (delay > 450)
    {
        return 115.0 + (d - 450.0) * 15.0 / 30.0;
    }
    if (delay > 180)
    {
        return 50.0 + (d - 180.0) * 65.0 / 270.0;
    }
    return 50.0 + (d - 180.0) * 15.0 / 180.0;
}

// CalculateTPReturn: delay→base TP with math.floor (toward −∞).
inline auto CalculateTPReturn(const bool isPCOrPetFormula, const std::int32_t delay) -> std::int16_t
{
    const double tp = isPCOrPetFormula ? PCOrPetTPReturn(delay) : MobTPReturn(delay);
    return static_cast<std::int16_t>(std::floor(tp));
}

} // namespace tpreturnhelpers
