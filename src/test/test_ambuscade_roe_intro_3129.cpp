#include "test_ambuscade_roe_intro_3129.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade roe-intro 3129 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventFinishGorpaMasorpa formula for dual-wire checks (slice 3129):
//   if csid == 385 then xi.roe.onRecordTrigger(player, 499) end
auto inlineShouldTriggerRoEIntro(const int32 csid) -> bool
{
    return csid == 385;
}

// Compact dual-wire pin matching C++ capacity formula:
//   csid == EventCSIDIntro
auto pinShouldTriggerRoEIntro(const int32 csid) -> bool
{
    return csid == ambuscadehelpers::EventCSIDIntro;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldTriggerRoEIntro
// (Lua ambuscade onEventFinishGorpaMasorpa intro CSID → RoE 499 gate; slice 3129).
auto runAmbuscadeRoEIntro3129SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDIntro;
    using ambuscadehelpers::RoERecordSteppingIntoAnAmbuscade;
    using ambuscadehelpers::ShouldTriggerRoEIntro;

    bool ok = true;

    // Residual constant / 1005 / 2906 pins still hold under dual-wire.
    ok = expect(EventCSIDIntro == 385, "EventCSIDIntro == 385") && ok;
    ok = expect(RoERecordSteppingIntoAnAmbuscade == 499,
                "RoERecordSteppingIntoAnAmbuscade == 499") &&
         ok;
    ok = expect(ShouldTriggerRoEIntro(EventCSIDIntro), "residual: csid 385 → trigger") && ok;
    ok = expect(ShouldTriggerRoEIntro(385), "residual: literal 385 → trigger") && ok;
    ok = expect(!ShouldTriggerRoEIntro(386), "residual: csid 386 → skip") && ok;
    ok = expect(!ShouldTriggerRoEIntro(0), "residual: csid 0 → skip") && ok;

    // Truth table poles + neighbors around EventCSIDIntro.
    const struct
    {
        int32       csid;
        bool        want;
        const char* label;
    } cases[] = {
        { EventCSIDIntro, true, "EventCSIDIntro → trigger" },
        { 385, true, "literal 385 → trigger" },
        { 386, false, "Gorpa menu → skip" },
        { 384, false, "RoE reminder → skip" },
        { 0, false, "zero → skip" },
        { 384, false, "one below → skip" },
        { 386, false, "one above → skip" },
        { -1, false, "negative → skip" },
        { 378, false, "tome enter → skip" },
        { 374, false, "tome register → skip" },
        { 10001, false, "exit CSID → skip" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldTriggerRoEIntro(c.csid);
        const bool inlineGot = inlineShouldTriggerRoEIntro(c.csid);
        const bool pinGot    = pinShouldTriggerRoEIntro(c.csid);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
        ok = expect(got == (c.csid == EventCSIDIntro), "dual-wire free == (csid == EventCSIDIntro)") && ok;
        ok = expect(got == (c.csid == 385), "dual-wire free == (csid == 385)") && ok;
    }

    // Host compose: inject finish csid then pure gate; onRecordTrigger remains host-owned.
    const struct
    {
        int32       csid;
        bool        wantTrigger;
        const char* label;
    } composeCases[] = {
        { EventCSIDIntro, true, "compose 385 → RoE trigger 499" },
        { 385, true, "compose literal 385 → RoE trigger 499" },
        { 386, false, "compose Gorpa menu → skip trigger" },
        { 384, false, "compose RoE reminder → skip trigger" },
        { 0, false, "compose other → skip trigger" },
        { 384, false, "compose one below → skip trigger" },
        { 386, false, "compose one above → skip trigger" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldTriggerRoEIntro(c.csid);
        const bool inlineGot = inlineShouldTriggerRoEIntro(c.csid);
        const bool pinGot    = pinShouldTriggerRoEIntro(c.csid);

        ok = expect(got == c.wantTrigger, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "compose dual-wire free == pin") && ok;
        ok = expect(got == (c.csid == EventCSIDIntro), "compose dual-wire free == (csid == EventCSIDIntro)") && ok;

        // When gate is true, host would call onRecordTrigger(499).
        if (got)
        {
            ok = expect(RoERecordSteppingIntoAnAmbuscade == 499, "host RoE record pin == 499") && ok;
        }
    }

    // Dense representative poles: free == inline == pin for residual CSID space.
    for (const int32 csid : { EventCSIDIntro, static_cast<int32>(385), static_cast<int32>(386),
                              static_cast<int32>(384), static_cast<int32>(0), static_cast<int32>(-1),
                              static_cast<int32>(378), static_cast<int32>(374), static_cast<int32>(10001) })
    {
        const bool got = ShouldTriggerRoEIntro(csid);
        ok             = expect(got == pinShouldTriggerRoEIntro(csid), "dense free == pin") && ok;
        ok             = expect(got == inlineShouldTriggerRoEIntro(csid), "dense free == inline") && ok;
        ok             = expect(got == (csid == EventCSIDIntro), "dense free == (csid == EventCSIDIntro)") && ok;
    }

    // Explicit composition pins matching C++ capacity.
    ok = expect(ShouldTriggerRoEIntro(EventCSIDIntro) == true, "compose intro CSID") && ok;
    ok = expect(ShouldTriggerRoEIntro(386) == false, "compose Gorpa menu") && ok;

    return ok;
}
