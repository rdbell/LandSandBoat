#pragma once

#include <cstdint>

// Pure utils.getEcosystemStrengthBonus systemStrengthTable.
// Parity: internal/ecosystem StrengthBonus (slice 1590 production wire).

namespace ecosystemstrengthhelpers
{

// xi.ecosystem / xi::Ecosystem ordinals used by the strength table.
constexpr std::uint8_t EcoAmorph    = 1;
constexpr std::uint8_t EcoAquan     = 2;
constexpr std::uint8_t EcoArcana    = 3;
constexpr std::uint8_t EcoBeast     = 5;
constexpr std::uint8_t EcoBird      = 7;
constexpr std::uint8_t EcoDemon     = 8;
constexpr std::uint8_t EcoDragon    = 9;
constexpr std::uint8_t EcoLizard    = 13;
constexpr std::uint8_t EcoLuminian  = 14;
constexpr std::uint8_t EcoLuminion  = 15;
constexpr std::uint8_t EcoPlantoid  = 16;
constexpr std::uint8_t EcoUndead    = 18;
constexpr std::uint8_t EcoVermin    = 19;

// Returns +1 strong, -1 weak, 0 default (utils.getEcosystemStrengthBonus).
constexpr auto StrengthBonus(const std::uint8_t attacker, const std::uint8_t defender) -> std::int8_t
{
    switch (attacker)
    {
        case EcoBeast:
            if (defender == EcoLizard)
            {
                return 1;
            }
            if (defender == EcoPlantoid)
            {
                return -1;
            }
            return 0;
        case EcoLizard:
            if (defender == EcoVermin)
            {
                return 1;
            }
            if (defender == EcoBeast)
            {
                return -1;
            }
            return 0;
        case EcoVermin:
            if (defender == EcoPlantoid)
            {
                return 1;
            }
            if (defender == EcoLizard)
            {
                return -1;
            }
            return 0;
        case EcoPlantoid:
            if (defender == EcoBeast)
            {
                return 1;
            }
            if (defender == EcoVermin)
            {
                return -1;
            }
            return 0;
        case EcoAquan:
            if (defender == EcoAmorph)
            {
                return 1;
            }
            if (defender == EcoBird)
            {
                return -1;
            }
            return 0;
        case EcoAmorph:
            if (defender == EcoBird)
            {
                return 1;
            }
            if (defender == EcoAquan)
            {
                return -1;
            }
            return 0;
        case EcoBird:
            if (defender == EcoAquan)
            {
                return 1;
            }
            if (defender == EcoAmorph)
            {
                return -1;
            }
            return 0;
        case EcoUndead:
            return defender == EcoArcana ? 1 : 0;
        case EcoArcana:
            return defender == EcoUndead ? 1 : 0;
        case EcoDragon:
            return defender == EcoDemon ? 1 : 0;
        case EcoDemon:
            return defender == EcoDragon ? 1 : 0;
        case EcoLuminian:
            return defender == EcoLuminion ? 1 : 0;
        case EcoLuminion:
            return defender == EcoLuminian ? 1 : 0;
        default:
            return 0;
    }
}

// Pet ATT/ACC/RACC path: apply ENHANCES_MONSTER_CORRELATION when bonus > 0.
constexpr auto HasMonsterCorrelationAdvantage(const std::uint8_t attacker, const std::uint8_t defender) -> bool
{
    return StrengthBonus(attacker, defender) > 0;
}

} // namespace ecosystemstrengthhelpers
