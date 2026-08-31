#include "test_status_effect_erase_all_8541.h"

#include "map/status_effect_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect erase all 8541 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectEraseAll8541SelfTests() -> bool
{
    using statuseffecthelpers::IsErasableCandidate;

    bool ok = true;
    ok = expect(IsErasableCandidate(true, true, false), "active erasable timed effect") && ok;
    ok = expect(!IsErasableCandidate(true, false, false), "permanent effect retained") && ok;
    ok = expect(!IsErasableCandidate(true, true, true), "already deleted effect retained") && ok;
    ok = expect(!IsErasableCandidate(false, true, false), "non-erasable effect retained") && ok;
    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-erase-all-8541", runStatusEffectEraseAll8541SelfTests);
