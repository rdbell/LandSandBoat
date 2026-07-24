#include "test_state_set_target_6317.h"

#include "map/ai/states/state_set_target.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "state SetTarget 6317 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineShouldUpdate(const bool hasTarget, const uint16 stored, const uint16 newID, const uint16 entity) -> bool
{
    return !hasTarget || newID != stored || (hasTarget && entity != newID);
}

} // namespace

// Pure dual-wire suite for CState::SetTarget (OmegaXI internal/aistate; slice 6317).
auto runStateSetTarget6317SelfTests() -> bool
{
    using statehelpers::ShouldUpdateTarget;

    bool ok = true;

    ok = expect(ShouldUpdateTarget(false, 1, 1, 0), "no target must update") && ok;
    ok = expect(!ShouldUpdateTarget(true, 5, 5, 5), "stable target must not update") && ok;
    ok = expect(ShouldUpdateTarget(true, 5, 9, 5), "new targid differs → update") && ok;
    ok = expect(ShouldUpdateTarget(true, 5, 5, 7), "entity targid drift → update") && ok;

    for (const bool has : { false, true })
    {
        for (const uint16 stored : { uint16{ 0 }, uint16{ 1 }, uint16{ 5 } })
        {
            for (const uint16 newID : { uint16{ 0 }, uint16{ 1 }, uint16{ 5 }, uint16{ 9 } })
            {
                for (const uint16 entity : { uint16{ 0 }, uint16{ 1 }, uint16{ 5 }, uint16{ 7 } })
                {
                    const bool got     = ShouldUpdateTarget(has, stored, newID, entity);
                    const bool inlineF = inlineShouldUpdate(has, stored, newID, entity);
                    const bool want    = !has || newID != stored || (has && entity != newID);
                    ok                 = expect(got == want, "free==want") && ok;
                    ok                 = expect(got == inlineF, "free==inline") && ok;
                }
            }
        }
    }

    return ok;
}
