#pragma once

#include "data/enums/damage_type.h"
#include "data/enums/status_effect.h"
#include "enums/action/react_kind.h"

#include <cstdint>

// Pure GetSpikes/Enspell/Rune damage-type and element affinity maps + Liement.
// Parity: internal/damagetype, internal/liement
//
// ENSPELL and ELEMENT are injected as their underlying integer ordinals so this
// header does not depend on battleutils.h (avoids circular include with the
// production TU that owns those enums).

namespace damageaffinityhelpers
{

// ENSPELL ordinals (battleutils.h)
constexpr std::uint8_t EnspellNone     = 0;
constexpr std::uint8_t EnspellIFire    = 1;
constexpr std::uint8_t EnspellIIce     = 2;
constexpr std::uint8_t EnspellIWind    = 3;
constexpr std::uint8_t EnspellIEarth   = 4;
constexpr std::uint8_t EnspellIThunder = 5;
constexpr std::uint8_t EnspellIWater   = 6;
constexpr std::uint8_t EnspellILight   = 7;
constexpr std::uint8_t EnspellIDark    = 8;
constexpr std::uint8_t EnspellIIFire   = 9;
constexpr std::uint8_t EnspellIIIce    = 10;
constexpr std::uint8_t EnspellIIWind   = 11;
constexpr std::uint8_t EnspellIIEarth  = 12;
constexpr std::uint8_t EnspellIIThunder = 13;
constexpr std::uint8_t EnspellIIWater  = 14;
constexpr std::uint8_t EnspellIILight  = 15;
constexpr std::uint8_t EnspellIIDark   = 16;

// ELEMENT ordinals (battleutils.h)
constexpr std::uint8_t ElementNone    = 0;
constexpr std::uint8_t ElementFire    = 1;
constexpr std::uint8_t ElementIce     = 2;
constexpr std::uint8_t ElementWind    = 3;
constexpr std::uint8_t ElementEarth   = 4;
constexpr std::uint8_t ElementThunder = 5;
constexpr std::uint8_t ElementWater   = 6;
constexpr std::uint8_t ElementLight   = 7;
constexpr std::uint8_t ElementDark    = 8;

inline auto GetSpikesDamageType(const ActionReactKind spikesType) -> xi::DamageType
{
    switch (spikesType)
    {
        case ActionReactKind::BlazeSpikes:
            return xi::DamageType::Fire;
        case ActionReactKind::IceSpikes:
            return xi::DamageType::Ice;
        case ActionReactKind::DreadSpikes:
            return xi::DamageType::Dark;
        case ActionReactKind::CurseSpikes:
            return xi::DamageType::None;
        case ActionReactKind::ShockSpikes:
            return xi::DamageType::Thunder;
        case ActionReactKind::ReprisalSpikes:
            return xi::DamageType::Light;
        case ActionReactKind::WindSpikes:
            return xi::DamageType::Wind;
        case ActionReactKind::EarthSpikes:
            return xi::DamageType::Earth;
        case ActionReactKind::WaterSpikes:
            return xi::DamageType::Water;
        case ActionReactKind::DeathSpikes:
            return xi::DamageType::Dark;
        default:
            return xi::DamageType::None;
    }
}

inline auto GetEnspellDamageType(const std::uint8_t enspellType) -> xi::DamageType
{
    switch (enspellType)
    {
        case EnspellIFire:
        case EnspellIIFire:
            return xi::DamageType::Fire;
        case EnspellIIce:
        case EnspellIIIce:
            return xi::DamageType::Ice;
        case EnspellIWind:
        case EnspellIIWind:
            return xi::DamageType::Wind;
        case EnspellIEarth:
        case EnspellIIEarth:
            return xi::DamageType::Earth;
        case EnspellIThunder:
        case EnspellIIThunder:
            return xi::DamageType::Thunder;
        case EnspellIWater:
        case EnspellIIWater:
            return xi::DamageType::Water;
        case EnspellILight:
        case EnspellIILight:
            return xi::DamageType::Light;
        case EnspellIDark:
        case EnspellIIDark:
            return xi::DamageType::Dark;
        default:
            return xi::DamageType::None;
    }
}

inline auto GetRuneEnhancementDamageType(const xi::StatusEffect runeEffect) -> xi::DamageType
{
    switch (runeEffect)
    {
        case xi::StatusEffect::Ignis:
            return xi::DamageType::Fire;
        case xi::StatusEffect::Gelus:
            return xi::DamageType::Ice;
        case xi::StatusEffect::Flabra:
            return xi::DamageType::Wind;
        case xi::StatusEffect::Tellus:
            return xi::DamageType::Earth;
        case xi::StatusEffect::Sulpor:
            return xi::DamageType::Thunder;
        case xi::StatusEffect::Unda:
            return xi::DamageType::Water;
        case xi::StatusEffect::Lux:
            return xi::DamageType::Light;
        case xi::StatusEffect::Tenebrae:
            return xi::DamageType::Dark;
        default:
            return xi::DamageType::None;
    }
}

// Returns ELEMENT ordinal (battleutils.h ELEMENT_*).
inline auto GetRuneEnhancementElement(const xi::StatusEffect runeEffect) -> std::uint8_t
{
    switch (runeEffect)
    {
        case xi::StatusEffect::Ignis:
            return ElementFire;
        case xi::StatusEffect::Gelus:
            return ElementIce;
        case xi::StatusEffect::Flabra:
            return ElementWind;
        case xi::StatusEffect::Tellus:
            return ElementEarth;
        case xi::StatusEffect::Sulpor:
            return ElementThunder;
        case xi::StatusEffect::Unda:
            return ElementWater;
        case xi::StatusEffect::Lux:
            return ElementLight;
        case xi::StatusEffect::Tenebrae:
            return ElementDark;
        default:
            return ElementNone;
    }
}

struct LiementAbsorbResult
{
    float multiplier = 1.0f;
    bool  consume    = false;
};

constexpr std::uint16_t LiementBaseAbsorbPercent = 85;
constexpr int           LiementMaxPackedRunes    = 4;

constexpr auto CheckLiementAbsorb(const bool          active,
                                  const std::uint16_t absorbPower,
                                  const std::uint16_t absorbTypeBits,
                                  const std::uint16_t damageType) -> LiementAbsorbResult
{
    if (!active)
    {
        return {};
    }
    std::uint16_t count = 0;
    for (int i = 0; i < LiementMaxPackedRunes; ++i)
    {
        const auto packed = static_cast<std::uint16_t>((absorbTypeBits >> (i * 4)) & 0xF);
        if (packed == damageType)
        {
            ++count;
        }
    }
    if (count == 0)
    {
        return {};
    }
    const auto mult = static_cast<float>(
        static_cast<double>(LiementBaseAbsorbPercent + count * absorbPower) / 100.0);
    return LiementAbsorbResult{ mult * -1.f, true };
}

} // namespace damageaffinityhelpers
