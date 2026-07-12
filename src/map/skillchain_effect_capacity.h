#pragma once

#include <algorithm>
#include <cstdint>
#include <list>
#include <vector>

// Pure GetSkillChainEffect decision halves (status mutation remains host-side).
// FormSkillchain / GetSkillchainTier / GetSkillchainSubeffect remain in battleutils.

namespace skillchaineffecthelpers
{

constexpr auto CombinedWSProperties(const std::uint8_t primary, const std::uint8_t secondary, const std::uint8_t tertiary) -> std::uint16_t
{
    return static_cast<std::uint16_t>(primary) | (static_cast<std::uint16_t>(secondary) << 4) | (static_cast<std::uint16_t>(tertiary) << 8);
}

constexpr auto UnpackOpeningProperties(const std::uint16_t power, std::uint8_t& primary, std::uint8_t& secondary, std::uint8_t& tertiary) -> void
{
    primary   = static_cast<std::uint8_t>(power & 0b1111);
    secondary = static_cast<std::uint8_t>((power >> 4) & 0b1111);
    tertiary  = static_cast<std::uint8_t>((power >> 8) & 0b1111);
}

constexpr auto CapLinkedSubPower(const std::uint16_t subPower) -> std::uint16_t
{
    const auto n = static_cast<std::uint16_t>(subPower + 1);
    return n > 5 ? static_cast<std::uint16_t>(5) : n;
}

// SKILLCHAIN_ELEMENT ordinals used by Chainbound resonance (match battle_entity.h).
constexpr std::uint8_t SC_NONE          = 0;
constexpr std::uint8_t SC_LIQUEFACTION  = 3;
constexpr std::uint8_t SC_COMPRESSION   = 2;
constexpr std::uint8_t SC_REVERBERATION = 5;
constexpr std::uint8_t SC_INDURATION    = 7;
constexpr std::uint8_t SC_IMPACTION     = 8;
constexpr std::uint8_t SC_GRAVITATION   = 9;
constexpr std::uint8_t SC_DISTORTION    = 10;
constexpr std::uint8_t SC_FUSION        = 11;
constexpr std::uint8_t SC_FRAGMENTATION = 12;
constexpr std::uint8_t SC_LIGHT         = 13;
constexpr std::uint8_t SC_DARKNESS      = 14;

inline auto AppendChainboundResonance(const std::uint16_t chainboundPower, std::list<std::uint8_t>& out) -> void
{
    if (chainboundPower > 1)
    {
        out.emplace_back(SC_LIGHT);
        out.emplace_back(SC_DARKNESS);
        out.emplace_back(SC_GRAVITATION);
        out.emplace_back(SC_FRAGMENTATION);
        out.emplace_back(SC_DISTORTION);
        out.emplace_back(SC_FUSION);
    }
    out.emplace_back(SC_LIQUEFACTION);
    out.emplace_back(SC_INDURATION);
    out.emplace_back(SC_REVERBERATION);
    out.emplace_back(SC_IMPACTION);
    out.emplace_back(SC_COMPRESSION);
}

enum class SkillChainEffectOutcome : std::uint8_t
{
    OpenInitial = 0,
    EarlyNone,
    Link,
    ResetOpen,
};

struct SkillChainEffectResult
{
    SkillChainEffectOutcome outcome              = SkillChainEffectOutcome::EarlyNone;
    bool                    consumeChainbound    = false;
    bool                    ensureSkillchainEffect = false;
    std::uint16_t           combined             = 0;
    std::uint8_t            formed               = SC_NONE; // SKILLCHAIN_ELEMENT ordinal
    std::uint8_t            newTier              = 0;
    std::uint16_t           newPower             = 0;
    std::uint16_t           newSubPower          = 0;
    bool                    shrinkDurationBy1s   = false;
    bool                    resetDuration10s     = false;
    // formedSubeffect / res-rank apply left to host using battleutils helpers.
};

// formSkillchainFn(resonance, skill) -> formed element ordinal.
// getTierFn(element) -> tier.
template <typename FormFn, typename TierFn>
inline auto ResolveSkillChainEffect(const bool        hasSkillchain,
                                    const bool        hasChainbound,
                                    const bool        chainboundElapsedOK,
                                    const bool        skillchainElapsedOK,
                                    const std::uint16_t chainboundPower,
                                    const std::uint8_t  scTier,
                                    const std::uint16_t scPower,
                                    const std::uint16_t scSubPower,
                                    const std::uint8_t  primary,
                                    const std::uint8_t  secondary,
                                    const std::uint8_t  tertiary,
                                    FormFn              formSkillchainFn,
                                    TierFn              getTierFn) -> SkillChainEffectResult
{
    SkillChainEffectResult r{};
    r.combined = CombinedWSProperties(primary, secondary, tertiary);

    std::list<std::uint8_t> skillProperties = { primary, secondary, tertiary };

    if (!hasSkillchain && !hasChainbound)
    {
        r.outcome                = SkillChainEffectOutcome::OpenInitial;
        r.ensureSkillchainEffect = true;
        r.newTier                = 0;
        r.newPower               = r.combined;
        r.newSubPower            = 0;
        r.resetDuration10s       = true;
        return r;
    }

    std::uint8_t formed        = SC_NONE;
    bool         hasSC         = hasSkillchain;
    bool         consumeCB     = false;
    bool         ensureSC      = false;

    if (hasChainbound)
    {
        if (chainboundElapsedOK)
        {
            std::list<std::uint8_t> resonance;
            AppendChainboundResonance(chainboundPower, resonance);
            formed = formSkillchainFn(resonance, skillProperties);
        }
        consumeCB = true;
        ensureSC  = true;
        hasSC     = true;
    }
    else if (hasSkillchain && skillchainElapsedOK)
    {
        if (scTier == 0)
        {
            if (scPower == 0)
            {
                r.outcome = SkillChainEffectOutcome::EarlyNone;
                return r;
            }
            std::uint8_t p = 0;
            std::uint8_t s = 0;
            std::uint8_t t = 0;
            UnpackOpeningProperties(scPower, p, s, t);
            std::list<std::uint8_t> resonance = { p, s, t };
            formed                            = formSkillchainFn(resonance, skillProperties);
        }
        else
        {
            std::list<std::uint8_t> resonance = { static_cast<std::uint8_t>(scPower) };
            formed                            = formSkillchainFn(resonance, skillProperties);
        }
    }

    if (!hasSC)
    {
        r.outcome           = SkillChainEffectOutcome::EarlyNone;
        r.consumeChainbound = consumeCB;
        return r;
    }

    r.consumeChainbound      = consumeCB;
    r.ensureSkillchainEffect = ensureSC;
    r.formed                 = formed;

    if (formed != SC_NONE)
    {
        r.outcome            = SkillChainEffectOutcome::Link;
        r.newTier            = getTierFn(formed);
        r.newPower           = formed;
        r.newSubPower        = CapLinkedSubPower(scSubPower);
        r.shrinkDurationBy1s = true;
        return r;
    }

    r.outcome          = SkillChainEffectOutcome::ResetOpen;
    r.newTier          = 0;
    r.newPower         = r.combined;
    r.newSubPower      = 0;
    r.resetDuration10s = true;
    return r;
}

} // namespace skillchaineffecthelpers
