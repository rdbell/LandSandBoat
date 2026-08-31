#include "test_status_effect_charm_gain_side_effects_8240.h"

#include "map/status_effect_capacity.h"
#include "map/status_effect.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect charm gain side effects 8240 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectCharmGainSideEffects8240SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(IsCharmEffectForGainSideEffects(StatusIDCharmI), "CharmI matches") && ok;
    ok = expect(IsCharmEffectForGainSideEffects(StatusIDCharmIi), "CharmIi matches") && ok;
    ok = expect(!IsCharmEffectForGainSideEffects(static_cast<uint16>(xi::StatusEffect::Poison)), "Poison does not match") && ok;
    ok = expect(!IsCharmEffectForGainSideEffects(static_cast<uint16>(xi::StatusEffect::None)), "None does not match") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-charm-gain-side-effects-8240", runStatusEffectCharmGainSideEffects8240SelfTests);
