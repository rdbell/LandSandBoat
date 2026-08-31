#include "test_status_effect_kill_timed_8245.h"

#include "map/status_effect_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect kill timed 8245 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectKillTimed8245SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;
    ok = expect(ShouldKillTimedEffect(true), "positive duration removed") && ok;
    ok = expect(!ShouldKillTimedEffect(false), "zero duration retained") && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-kill-timed-8245", runStatusEffectKillTimed8245SelfTests);
