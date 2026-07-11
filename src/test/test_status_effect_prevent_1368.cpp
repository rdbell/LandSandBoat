#include "test_status_effect_prevent_1368.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect prevent 1368 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "status effect prevent 1368 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runStatusEffectPrevent1368SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(IsPreventActionEffectID(StatusIDSleepI, true), "sleep ignore charm") && ok;
    ok = expect(IsPreventActionEffectID(StatusIDStun, false), "stun") && ok;
    ok = expect(IsPreventActionEffectID(StatusIDCharmI, false), "charm included") && ok;
    ok = expect(!IsPreventActionEffectID(StatusIDCharmI, true), "charm ignored") && ok;
    ok = expect(!IsPreventActionEffectID(StatusIDCharmIi, true), "charm2 ignored") && ok;
    ok = expect(IsPreventActionEffectID(StatusIDTerror, false), "terror") && ok;
    ok = expect(!IsPreventActionEffectID(3, false), "poison not prevent") && ok;

    ok = expect(IsAsleepEffectID(StatusIDSleepI) && IsAsleepEffectID(StatusIDSleepIi), "asleep sleep") && ok;
    ok = expect(IsAsleepEffectID(StatusIDLullaby) && !IsAsleepEffectID(StatusIDStun), "asleep lullaby") && ok;

    ok = expect(IsConfrontationFlag(true) && !IsConfrontationFlag(false), "conf flag") && ok;
    ok = expectEq(ConfrontationPowerOrZero(true, 42), static_cast<uint16>(42), "conf power") && ok;
    ok = expectEq(ConfrontationPowerOrZero(false, 42), static_cast<uint16>(0), "conf zero") && ok;
    ok = expect(ShouldCopyConfrontation(true) && !ShouldCopyConfrontation(false), "copy conf") && ok;

    return ok;
}
