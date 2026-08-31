#include "test_status_effect_steal_8540.h"

#include "map/status_effect_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect steal 8540 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectSteal8540SelfTests() -> bool
{
    using statuseffecthelpers::HasRemovableCandidates;
    using statuseffecthelpers::IsFlagRemovableCandidate;

    bool ok = true;
    ok = expect(IsFlagRemovableCandidate(true, true, false), "active flagged timed effect") && ok;
    ok = expect(!IsFlagRemovableCandidate(true, false, false), "permanent effect excluded") && ok;
    ok = expect(!IsFlagRemovableCandidate(true, true, true), "deleted effect excluded") && ok;
    ok = expect(!IsFlagRemovableCandidate(false, true, false), "unflagged effect excluded") && ok;
    ok = expect(HasRemovableCandidates(1), "nonempty candidate list") && ok;
    ok = expect(!HasRemovableCandidates(0), "empty candidate list") && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-steal-8540", runStatusEffectSteal8540SelfTests);
