#include "test_skillchain_effect_1537.h"

#include "map/skillchain_effect_capacity.h"

#include <iostream>
#include <list>

namespace
{
using skillchaineffecthelpers::CapLinkedSubPower;
using skillchaineffecthelpers::CombinedWSProperties;
using skillchaineffecthelpers::ResolveSkillChainEffect;
using skillchaineffecthelpers::SkillChainEffectOutcome;
using skillchaineffecthelpers::UnpackOpeningProperties;

// Identity form: returns first non-zero skill property when resonance non-empty (tests call with real form via injected lambda).
auto FormEcho(const std::list<std::uint8_t>& resonance, const std::list<std::uint8_t>& skill) -> std::uint8_t
{
    if (resonance.empty())
    {
        return 0;
    }
    for (const auto s : skill)
    {
        if (s != 0)
        {
            return s;
        }
    }
    return 0;
}

auto TierIdentity(const std::uint8_t el) -> std::uint8_t
{
    if (el == 0)
    {
        return 0;
    }
    if (el >= 15)
    {
        return 4;
    }
    if (el >= 13)
    {
        return 3;
    }
    if (el >= 9)
    {
        return 2;
    }
    return 1;
}

// Form that implements Light+Light → LightII (15) for link-cap test.
auto FormLightII(const std::list<std::uint8_t>& resonance, const std::list<std::uint8_t>& skill) -> std::uint8_t
{
    for (const auto r : resonance)
    {
        for (const auto s : skill)
        {
            if (r == 13 && s == 13)
            {
                return 15; // SC_LIGHT_II
            }
        }
    }
    return 0;
}

auto Check() -> bool
{
    if (CombinedWSProperties(3, 4, 5) != static_cast<std::uint16_t>(3 | (4 << 4) | (5 << 8)))
    {
        return false;
    }
    {
        std::uint8_t p = 0;
        std::uint8_t s = 0;
        std::uint8_t t = 0;
        UnpackOpeningProperties(CombinedWSProperties(3, 4, 5), p, s, t);
        if (p != 3 || s != 4 || t != 5)
        {
            return false;
        }
    }
    if (CapLinkedSubPower(0) != 1 || CapLinkedSubPower(4) != 5 || CapLinkedSubPower(5) != 5)
    {
        return false;
    }

    {
        const auto r = ResolveSkillChainEffect(false, false, false, false, 0, 0, 0, 0, 3, 0, 0, FormEcho, TierIdentity);
        if (r.outcome != SkillChainEffectOutcome::OpenInitial || r.newPower != CombinedWSProperties(3, 0, 0))
        {
            return false;
        }
    }
    {
        const auto r = ResolveSkillChainEffect(true, false, false, true, 0, 0, 0, 0, 3, 0, 0, FormEcho, TierIdentity);
        if (r.outcome != SkillChainEffectOutcome::EarlyNone)
        {
            return false;
        }
    }
    {
        const auto openPower = CombinedWSProperties(3, 0, 0);
        const auto r         = ResolveSkillChainEffect(true, false, false, true, 0, 0, openPower, 0, 4, 0, 0, FormEcho, TierIdentity);
        if (r.outcome != SkillChainEffectOutcome::Link || r.formed != 4 || r.newSubPower != 1)
        {
            return false;
        }
    }
    {
        const auto openPower = CombinedWSProperties(3, 0, 0);
        const auto r         = ResolveSkillChainEffect(true, false, false, false, 0, 0, openPower, 2, 4, 0, 0, FormEcho, TierIdentity);
        if (r.outcome != SkillChainEffectOutcome::ResetOpen || r.newSubPower != 0 || r.newPower != CombinedWSProperties(4, 0, 0))
        {
            return false;
        }
    }
    {
        const auto r = ResolveSkillChainEffect(true, false, false, true, 0, 3, 13, 5, 13, 0, 0, FormLightII, TierIdentity);
        if (r.outcome != SkillChainEffectOutcome::Link || r.formed != 15 || r.newSubPower != 5 || r.newTier != 4)
        {
            return false;
        }
    }
    {
        const auto r = ResolveSkillChainEffect(false, true, true, false, 1, 0, 0, 0, 3, 0, 0, FormEcho, TierIdentity);
        if (!r.consumeChainbound || !r.ensureSkillchainEffect || r.outcome != SkillChainEffectOutcome::Link)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runSkillchainEffect1537SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "skillchain_effect_1537 self-tests failed\n";
        return false;
    }
    return true;
}
