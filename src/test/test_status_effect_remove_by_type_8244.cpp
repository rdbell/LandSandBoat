#include "test_status_effect_remove_by_type_8244.h"

#include "map/status_effect_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect remove by type 8244 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectRemoveByType8244SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(MatchesEffectType(7, 7), "matching nonzero type") && ok;
    ok = expect(!MatchesEffectType(0, 0), "zero type rejected") && ok;
    ok = expect(!MatchesEffectType(7, 8), "different type") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("status-effect-remove-by-type-8244", runStatusEffectRemoveByType8244SelfTests);
