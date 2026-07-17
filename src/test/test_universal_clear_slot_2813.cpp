#include "test_universal_clear_slot_2813.h"

#include "map/universal_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "universal clear slot 2813 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runUniversalClearSlot2813SelfTests() -> bool
{
    using namespace ucontainerhelpers;

    bool ok = true;

    // ShouldClearSlot: pure range gate
    ok = expect(ShouldClearSlot(true), "allow in-range") && ok;
    ok = expect(!ShouldClearSlot(false), "reject out-of-range") && ok;

    // ClearSlot does not adjust m_count (parity quirk vs SetItem null)
    ok = expect(!ShouldAdjustCountOnClearSlot(), "ClearSlot never adjusts count") && ok;

    return ok;
}
