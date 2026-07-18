#include "test_attackutils_occ_damage_3907.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{
auto expectEq(const uint32 got, const uint32 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "attackutils Occ damage 3907 self-test failed: " << label << " got " << got << " want " << want << '\n';
        return false;
    }
    return true;
}
} // namespace

auto runAttackutilsOccDamage3907SelfTests() -> bool
{
    using attackutilshelpers::ApplyOccProcDamage;
    using attackutilshelpers::OccProcResult;
    bool ok = true;
    ok = expectEq(ApplyOccProcDamage(100, OccProcResult::RemTriple, 0.0f), 300, "rem triple") && ok;
    ok = expectEq(ApplyOccProcDamage(100, OccProcResult::RemDouble, 0.0f), 200, "rem double") && ok;
    ok = expectEq(ApplyOccProcDamage(101, OccProcResult::ExtraDamage, 2.5f), 252, "extra fractional truncates") && ok;
    ok = expectEq(ApplyOccProcDamage(1, OccProcResult::ExtraDamage, 0.5f), 0, "extra subunit truncates") && ok;
    ok = expectEq(ApplyOccProcDamage(100, OccProcResult::None, 9.0f), 100, "none passthrough") && ok;
    return ok;
}
