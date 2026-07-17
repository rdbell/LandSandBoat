#include "test_universal_slot_empty_2822.h"

#include "map/universal_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "universal slot empty 2822 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runUniversalSlotEmpty2822SelfTests() -> bool
{
    using ucontainerhelpers::PlanIsSlotEmpty;

    bool ok = true;

    // Out of range is empty regardless of itemNull host value.
    ok = expect(PlanIsSlotEmpty(false, true), "OOR + null → empty") && ok;
    ok = expect(PlanIsSlotEmpty(false, false), "OOR + non-null → empty") && ok;

    // In range: empty iff item null.
    ok = expect(PlanIsSlotEmpty(true, true), "in-range null → empty") && ok;
    ok = expect(!PlanIsSlotEmpty(true, false), "in-range occupied → not empty") && ok;

    return ok;
}
