#include "test_ambuscade_intense_ve_3143.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade intense-ve 3143 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventFinishTome formula for dual-wire checks (slice 3143):
//   if csid == 374 and option == 5 then player:createInstance(30000) end
auto inlineShouldCreateIntenseVEInstance(const int32 csid, const int32 option) -> bool
{
    return csid == 374 && option == 5;
}

// Compact dual-wire pin matching C++ capacity formula:
//   csid == EventCSIDTomeRegister && option == TomeOptionIntenseVE
auto pinShouldCreateIntenseVEInstance(const int32 csid, const int32 option) -> bool
{
    return csid == ambuscadehelpers::EventCSIDTomeRegister &&
           option == ambuscadehelpers::TomeOptionIntenseVE;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldCreateIntenseVEInstance
// (Lua ambuscade onEventFinishTome Intense VE createInstance gate; slice 3143).
auto runAmbuscadeIntenseVE3143SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDTomeRegister;
    using ambuscadehelpers::InstanceIntenseVE;
    using ambuscadehelpers::ShouldCreateIntenseVEInstance;
    using ambuscadehelpers::TomeOptionIntenseVE;

    bool ok = true;

    // Residual constant / 1005 / 2910 pins still hold under dual-wire.
    ok = expect(EventCSIDTomeRegister == 374, "EventCSIDTomeRegister == 374") && ok;
    ok = expect(TomeOptionIntenseVE == 5, "TomeOptionIntenseVE == 5") && ok;
    ok = expect(InstanceIntenseVE == 30000, "InstanceIntenseVE == 30000") && ok;
    ok = expect(ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, TomeOptionIntenseVE),
                "residual: csid 374 option 5 → create") &&
         ok;
    ok = expect(ShouldCreateIntenseVEInstance(374, 5), "residual: literal 374 / 5 → create") && ok;
    ok = expect(!ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, 1),
                "residual: option 1 → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(378, TomeOptionIntenseVE),
                "residual: csid 378 → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(0, 0), "residual: 0 / 0 → skip") && ok;

    // Truth table poles: both csid and option must match.
    const struct
    {
        int32       csid;
        int32       option;
        bool        want;
        const char* label;
    } cases[] = {
        { EventCSIDTomeRegister, TomeOptionIntenseVE, true, "register Intense VE → create" },
        { 374, 5, true, "literal 374 / 5 → create" },
        { EventCSIDTomeRegister, 1, false, "Intense VD → skip" },
        { EventCSIDTomeRegister, 2, false, "Intense D → skip" },
        { EventCSIDTomeRegister, 3, false, "Intense N → skip" },
        { EventCSIDTomeRegister, 4, false, "Intense E → skip" },
        { EventCSIDTomeRegister, 6, false, "Regular VD → skip" },
        { EventCSIDTomeRegister, 7, false, "Regular D → skip" },
        { EventCSIDTomeRegister, 8, false, "Regular N → skip" },
        { EventCSIDTomeRegister, 9, false, "Regular E → skip" },
        { EventCSIDTomeRegister, 10, false, "Regular VE → skip" },
        { EventCSIDTomeRegister, 11, false, "Light → skip" },
        { 378, TomeOptionIntenseVE, false, "tome enter CSID → skip" },
        { 385, TomeOptionIntenseVE, false, "intro CSID → skip" },
        { 10001, TomeOptionIntenseVE, false, "exit CSID → skip" },
        { 0, TomeOptionIntenseVE, false, "zero csid → skip" },
        { EventCSIDTomeRegister, 0, false, "zero option → skip" },
        { -1, TomeOptionIntenseVE, false, "negative csid → skip" },
        { EventCSIDTomeRegister, -1, false, "negative option → skip" },
        { 373, 5, false, "one below register CSID → skip" },
        { 375, 5, false, "one above register CSID → skip" },
        { 374, 4, false, "one below Intense VE option → skip" },
        { 374, 6, false, "one above Intense VE option → skip" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldCreateIntenseVEInstance(c.csid, c.option);
        const bool inlineGot = inlineShouldCreateIntenseVEInstance(c.csid, c.option);
        const bool pinGot    = pinShouldCreateIntenseVEInstance(c.csid, c.option);
        const bool wantF     = c.csid == EventCSIDTomeRegister && c.option == TomeOptionIntenseVE;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
        ok = expect(got == pinGot, "dual-wire free == C++ pin formula") && ok;
        ok = expect(got == wantF, "dual-wire free == (csid==EventCSIDTomeRegister && option==TomeOptionIntenseVE)") &&
             ok;
        ok = expect(got == (c.csid == 374 && c.option == 5), "dual-wire free == (csid==374 && option==5)") &&
             ok;
    }

    // Host compose: inject finish csid + option then pure gate; createInstance remains host-owned.
    const struct
    {
        int32       csid;
        int32       option;
        bool        wantCreate;
        const char* label;
    } composeCases[] = {
        { EventCSIDTomeRegister, TomeOptionIntenseVE, true, "compose 374 / 5 → createInstance 30000" },
        { 374, 5, true, "compose literal 374 / 5 → createInstance 30000" },
        { EventCSIDTomeRegister, 1, false, "compose Intense VD → skip create" },
        { EventCSIDTomeRegister, 4, false, "compose Intense E → skip create" },
        { EventCSIDTomeRegister, 10, false, "compose Regular VE → skip create" },
        { EventCSIDTomeRegister, 11, false, "compose Light → skip create" },
        { 378, TomeOptionIntenseVE, false, "compose tome enter → skip create" },
        { 0, 0, false, "compose other → skip create" },
        { 373, 5, false, "compose one below CSID → skip create" },
        { 375, 5, false, "compose one above CSID → skip create" },
        { 374, 4, false, "compose one below option → skip create" },
        { 374, 6, false, "compose one above option → skip create" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldCreateIntenseVEInstance(c.csid, c.option);
        const bool inlineGot = inlineShouldCreateIntenseVEInstance(c.csid, c.option);
        const bool pinGot    = pinShouldCreateIntenseVEInstance(c.csid, c.option);

        ok = expect(got == c.wantCreate, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "compose dual-wire free == pin") && ok;
        ok = expect(got == (c.csid == EventCSIDTomeRegister && c.option == TomeOptionIntenseVE),
                    "compose dual-wire free == formula") &&
             ok;

        // When gate is true, host would call createInstance(30000).
        if (got)
        {
            ok = expect(InstanceIntenseVE == 30000, "host instance pin == 30000") && ok;
        }
    }

    // Dense representative poles: free == inline == pin for residual domain.
    const struct
    {
        int32 csid;
        int32 option;
    } dense[] = {
        { EventCSIDTomeRegister, TomeOptionIntenseVE },
        { 374, 5 },
        { EventCSIDTomeRegister, 1 },
        { EventCSIDTomeRegister, 4 },
        { EventCSIDTomeRegister, 6 },
        { EventCSIDTomeRegister, 10 },
        { EventCSIDTomeRegister, 11 },
        { 378, TomeOptionIntenseVE },
        { 385, TomeOptionIntenseVE },
        { 10001, TomeOptionIntenseVE },
        { 0, TomeOptionIntenseVE },
        { EventCSIDTomeRegister, 0 },
        { -1, TomeOptionIntenseVE },
        { EventCSIDTomeRegister, -1 },
        { 373, 5 },
        { 375, 5 },
        { 374, 4 },
        { 374, 6 },
        { 0, 0 },
    };

    for (const auto& p : dense)
    {
        const bool got = ShouldCreateIntenseVEInstance(p.csid, p.option);
        ok             = expect(got == pinShouldCreateIntenseVEInstance(p.csid, p.option), "dense free == pin") &&
             ok;
        ok = expect(got == inlineShouldCreateIntenseVEInstance(p.csid, p.option), "dense free == inline") &&
             ok;
        ok = expect(got == (p.csid == EventCSIDTomeRegister && p.option == TomeOptionIntenseVE),
                    "dense free == formula") &&
             ok;
    }

    // Explicit composition pins matching C++ capacity.
    ok = expect(ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, TomeOptionIntenseVE) == true,
                "compose register Intense VE") &&
         ok;
    ok = expect(ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, 1) == false, "compose Intense VD") &&
         ok;

    return ok;
}
