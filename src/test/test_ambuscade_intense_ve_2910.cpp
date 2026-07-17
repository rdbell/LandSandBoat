#include "test_ambuscade_intense_ve_2910.h"

#include "map/ambuscade_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "ambuscade intense-ve 2910 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onEventFinishTome formula for dual-wire checks:
//   if csid == 374 and option == 5 then player:createInstance(30000) end
auto inlineShouldCreateIntenseVEInstance(const int32 csid, const int32 option) -> bool
{
    return csid == 374 && option == 5;
}

} // namespace

// Pure dual-wire expansion for ambuscadehelpers::ShouldCreateIntenseVEInstance
// (Lua ambuscade onEventFinishTome Intense VE createInstance gate).
auto runAmbuscadeIntenseVE2910SelfTests() -> bool
{
    using ambuscadehelpers::EventCSIDTomeRegister;
    using ambuscadehelpers::InstanceIntenseVE;
    using ambuscadehelpers::ShouldCreateIntenseVEInstance;
    using ambuscadehelpers::TomeOptionIntenseVE;

    bool ok = true;

    // Constant pins.
    ok = expect(EventCSIDTomeRegister == 374, "EventCSIDTomeRegister == 374") && ok;
    ok = expect(TomeOptionIntenseVE == 5, "TomeOptionIntenseVE == 5") && ok;
    ok = expect(InstanceIntenseVE == 30000, "InstanceIntenseVE == 30000") && ok;

    // Truth table for ShouldCreateIntenseVEInstance (csid == 374 && option == 5).
    ok = expect(ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, TomeOptionIntenseVE),
                "csid 374 option 5 → create") &&
         ok;
    ok = expect(ShouldCreateIntenseVEInstance(374, 5), "literal 374 / 5 → create") && ok;
    ok = expect(!ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, 1),
                "csid 374 option 1 (Intense VD) → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, 4),
                "csid 374 option 4 (Intense E) → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, 6),
                "csid 374 option 6 (Regular VD) → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, 10),
                "csid 374 option 10 (Regular VE) → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, 11),
                "csid 374 option 11 (Light) → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(378, TomeOptionIntenseVE),
                "csid 378 option 5 → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(0, TomeOptionIntenseVE),
                "csid 0 option 5 → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, 0),
                "csid 374 option 0 → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(-1, TomeOptionIntenseVE),
                "csid -1 option 5 → skip") &&
         ok;
    ok = expect(!ShouldCreateIntenseVEInstance(EventCSIDTomeRegister, -1),
                "csid 374 option -1 → skip") &&
         ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        int32       csid;
        int32       option;
        bool        want;
        const char* label;
    } cases[] = {
        { 374, 5, true, "table register Intense VE" },
        { 374, 1, false, "table Intense VD" },
        { 374, 4, false, "table Intense E" },
        { 374, 6, false, "table Regular VD" },
        { 374, 10, false, "table Regular VE" },
        { 374, 11, false, "table Light" },
        { 378, 5, false, "table tome enter CSID" },
        { 0, 5, false, "table zero csid" },
        { 374, 0, false, "table zero option" },
        { -1, 5, false, "table negative csid" },
        { 374, -1, false, "table negative option" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldCreateIntenseVEInstance(c.csid, c.option);
        const bool inlineGot = inlineShouldCreateIntenseVEInstance(c.csid, c.option);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;
    }

    // Host compose: inject finish csid + option then pure gate.
    // When gate is true, host calls createInstance(30000); when false, skips.
    const struct
    {
        int32       csid;
        int32       option;
        bool        wantCreate;
        const char* label;
    } composeCases[] = {
        { 374, 5, true, "compose 374 / 5 → createInstance 30000" },
        { 374, 1, false, "compose 374 / 1 → skip create" },
        { 378, 5, false, "compose 378 / 5 → skip create" },
        { 0, 0, false, "compose other → skip create" },
    };

    for (const auto& c : composeCases)
    {
        const bool got       = ShouldCreateIntenseVEInstance(c.csid, c.option);
        const bool inlineGot = inlineShouldCreateIntenseVEInstance(c.csid, c.option);

        ok = expect(got == c.wantCreate, c.label) && ok;
        ok = expect(got == inlineGot, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
