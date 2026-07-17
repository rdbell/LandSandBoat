#include "test_ambuscade_tome_enter_2901.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade tome-enter 2901 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventFinishTome formula for dual-wire checks:
//   elseif csid == 378 then -- TODO end
auto inlineShouldHandleTomeEnterFinish(const int32 csid) -> bool
{
    return csid == 378;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldHandleTomeEnterFinish
// (Lua ambuscade onEventFinishTome enter CSID gate).
auto runAmbuscadeTomeEnter2901SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDTomeEnter;
    using ambuscadehelpers::ShouldHandleTomeEnterFinish;

    bool ok = true;

    // Constant pin.
    ok = expect(EventCSIDTomeEnter == 378, "EventCSIDTomeEnter == 378") && ok;

    // Truth table for ShouldHandleTomeEnterFinish (csid == 378).
    ok = expect(ShouldHandleTomeEnterFinish(EventCSIDTomeEnter), "csid 378 → handle") && ok;
    ok = expect(ShouldHandleTomeEnterFinish(378), "literal 378 → handle") && ok;
    ok = expect(!ShouldHandleTomeEnterFinish(374), "csid 374 (register) → skip") && ok;
    ok = expect(!ShouldHandleTomeEnterFinish(0), "csid 0 → skip") && ok;
    ok = expect(!ShouldHandleTomeEnterFinish(377), "csid 377 → skip") && ok;
    ok = expect(!ShouldHandleTomeEnterFinish(379), "csid 379 → skip") && ok;
    ok = expect(!ShouldHandleTomeEnterFinish(386), "csid 386 (Gorpa menu) → skip") && ok;
    ok = expect(!ShouldHandleTomeEnterFinish(-1), "csid -1 → skip") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        int32       csid;
        bool        want;
        const char* label;
    } cases[] = {
        { 378, true, "table enter CSID" },
        { 374, false, "table register CSID" },
        { 0, false, "table zero" },
        { 377, false, "table one below" },
        { 379, false, "table one above" },
        { 386, false, "table Gorpa menu" },
        { -1, false, "table negative" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldHandleTomeEnterFinish(c.csid);
        const bool inlineGot = inlineShouldHandleTomeEnterFinish(c.csid);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: inject finish csid then pure gate.
    // When gate is true, host enters the (TODO) enter body; when false, skips.
    const struct
    {
        int32       csid;
        bool        wantHandle;
        const char* label;
    } composeCases[] = {
        { 378, true, "compose 378 → enter body" },
        { 374, false, "compose 374 → skip body" },
        { 0, false, "compose other → skip body" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldHandleTomeEnterFinish(c.csid);
        const bool inlineGot = inlineShouldHandleTomeEnterFinish(c.csid);

        ok = expect(got == c.wantHandle, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
