#include "test_ambuscade_roe_intro_2906.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade roe-intro 2906 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventFinishGorpaMasorpa formula for dual-wire checks:
//   if csid == 385 then xi.roe.onRecordTrigger(player, 499) end
auto inlineShouldTriggerRoEIntro(const int32 csid) -> bool
{
    return csid == 385;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldTriggerRoEIntro
// (Lua ambuscade onEventFinishGorpaMasorpa intro CSID → RoE 499 gate).
auto runAmbuscadeRoEIntro2906SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDIntro;
    using ambuscadehelpers::RoERecordSteppingIntoAnAmbuscade;
    using ambuscadehelpers::ShouldTriggerRoEIntro;

    bool ok = true;

    // Constant pins.
    ok = expect(EventCSIDIntro == 385, "EventCSIDIntro == 385") && ok;
    ok = expect(RoERecordSteppingIntoAnAmbuscade == 499,
                "RoERecordSteppingIntoAnAmbuscade == 499") &&
         ok;

    // Truth table for ShouldTriggerRoEIntro (csid == 385).
    ok = expect(ShouldTriggerRoEIntro(EventCSIDIntro), "csid 385 → trigger") && ok;
    ok = expect(ShouldTriggerRoEIntro(385), "literal 385 → trigger") && ok;
    ok = expect(!ShouldTriggerRoEIntro(386), "csid 386 (menu) → skip") && ok;
    ok = expect(!ShouldTriggerRoEIntro(384), "csid 384 (RoE reminder) → skip") && ok;
    ok = expect(!ShouldTriggerRoEIntro(0), "csid 0 → skip") && ok;
    ok = expect(!ShouldTriggerRoEIntro(378), "csid 378 (tome enter) → skip") && ok;
    ok = expect(!ShouldTriggerRoEIntro(-1), "csid -1 → skip") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        int32       csid;
        bool        want;
        const char* label;
    } cases[] = {
        { 385, true, "table intro CSID" },
        { 386, false, "table menu CSID" },
        { 384, false, "table RoE reminder CSID" },
        { 0, false, "table zero" },
        { 384, false, "table one below" },
        { 386, false, "table one above" },
        { 378, false, "table tome enter" },
        { -1, false, "table negative" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldTriggerRoEIntro(c.csid);
        const bool inlineGot = inlineShouldTriggerRoEIntro(c.csid);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: inject finish csid then pure gate.
    // When gate is true, host calls onRecordTrigger(499); when false, skips.
    const struct
    {
        int32       csid;
        bool        wantTrigger;
        const char* label;
    } composeCases[] = {
        { 385, true, "compose 385 → RoE trigger 499" },
        { 386, false, "compose 386 → skip trigger" },
        { 384, false, "compose 384 → skip trigger" },
        { 0, false, "compose other → skip trigger" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldTriggerRoEIntro(c.csid);
        const bool inlineGot = inlineShouldTriggerRoEIntro(c.csid);

        ok = expect(got == c.wantTrigger, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
