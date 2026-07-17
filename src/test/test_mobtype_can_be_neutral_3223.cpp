#include "test_mobtype_can_be_neutral_3223.h"

#include "map/mobtype_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobtype CanBeNeutral 3223 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CMobEntity::CanBeNeutral type policy for dual-wire cross-check
// (dedicated slice 3223 expand residual 2934):
//   !Has(t, Notorious)
auto inlineCanBeNeutral(const uint8 t) -> bool
{
    return !mobtypehelpers::Has(t, mobtypehelpers::Notorious);
}

// Compact dual-wire pin matching Go pinCanBeNeutral3223 / C++ capacity
// direct form (prefer !Has over bool-split):
//   !Has(t, Notorious)
auto pinCanBeNeutral(const uint8 t) -> bool
{
    return !mobtypehelpers::Has(t, mobtypehelpers::Notorious);
}

} // namespace

// Pure dual-wire expansion for mobtypehelpers::CanBeNeutral
// (CMobEntity::CanBeNeutral type-bit gate; OmegaXI internal/mobtype;
// dedicated slice 3223 expand residual 2934 / prior 3076 / pure 2042/2655).
//
// Coverage:
//   - free == inline == pin == !Has(t, Notorious)
//   - residual poles: Normal, Notorious, Battlefield, NM|BF
//   - dense: zero type, all single known type bits, Notorious combinations
// Formula unchanged — not registered in CMake/main.
auto runMobtypeCanBeNeutral3223SelfTests() -> bool
{
    using mobtypehelpers::Battlefield;
    using mobtypehelpers::Called;
    using mobtypehelpers::CanBeNeutral;
    using mobtypehelpers::Event;
    using mobtypehelpers::Fished;
    using mobtypehelpers::Has;
    using mobtypehelpers::Normal;
    using mobtypehelpers::Notorious;

    bool ok = true;

    // Pin constants match MOBTYPE enum.
    ok = expect(Normal == 0x00, "pin Normal == 0x00") && ok;
    ok = expect(Notorious == 0x02, "pin Notorious == 0x02") && ok;
    ok = expect(Fished == 0x04, "pin Fished == 0x04") && ok;
    ok = expect(Called == 0x08, "pin Called == 0x08") && ok;
    ok = expect(Battlefield == 0x10, "pin Battlefield == 0x10") && ok;
    ok = expect(Event == 0x20, "pin Event == 0x20") && ok;

    // Residual 2042 / 2655 / 2934 / 3076 pins still hold under dual-wire.
    ok = expect(CanBeNeutral(Normal), "residual: no flags → true") && ok;
    ok = expect(!CanBeNeutral(Notorious), "residual: Notorious only → false") && ok;
    ok = expect(CanBeNeutral(Battlefield), "residual: Battlefield alone → true") && ok;
    ok = expect(!CanBeNeutral(static_cast<uint8>(Notorious | Battlefield)), "residual: NM|BF → false") && ok;

    // Other flags alone still true if not Notorious.
    ok = expect(CanBeNeutral(Fished), "Fished alone can be neutral") && ok;
    ok = expect(CanBeNeutral(Called), "Called alone can be neutral") && ok;
    ok = expect(CanBeNeutral(Event), "Event alone can be neutral") && ok;
    ok = expect(CanBeNeutral(0x01), "MOBTYPE_0X01 reserve can be neutral") && ok;
    ok = expect(CanBeNeutral(static_cast<uint8>(Fished | Called | Event)), "all neutral bits can be neutral") && ok;
    ok = expect(CanBeNeutral(static_cast<uint8>(Battlefield | Fished)), "Battlefield|Fished can be neutral") && ok;

    const struct
    {
        uint8       t;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual poles (slice 2934 / 3076 / 3223).
        { Normal, true, "no flags → true" },
        { 0x00, true, "explicit zero → true" },
        { Notorious, false, "Notorious only → false" },
        { Battlefield, true, "Battlefield alone → true" },
        { static_cast<uint8>(Notorious | Battlefield), false, "NM|BF → false" },

        // All single known type bits.
        { Fished, true, "Fished alone can be neutral" },
        { Called, true, "Called alone can be neutral" },
        { Event, true, "Event alone can be neutral" },
        { 0x01, true, "MOBTYPE_0X01 reserve can be neutral" },

        // Combinations with Notorious bit cleared.
        { static_cast<uint8>(Fished | Called), true, "Fished|Called can be neutral" },
        { static_cast<uint8>(Fished | Event), true, "Fished|Event can be neutral" },
        { static_cast<uint8>(Called | Event), true, "Called|Event can be neutral" },
        { static_cast<uint8>(Fished | Called | Event), true, "all neutral bits can be neutral" },
        { static_cast<uint8>(0x01 | Fished), true, "0x01|Fished can be neutral" },
        { static_cast<uint8>(0x01 | Called | Event), true, "0x01|Called|Event can be neutral" },
        { static_cast<uint8>(Battlefield | Fished), true, "Battlefield|Fished can be neutral" },
        { static_cast<uint8>(Battlefield | Called), true, "Battlefield|Called can be neutral" },
        { static_cast<uint8>(Battlefield | Event), true, "Battlefield|Event can be neutral" },
        { static_cast<uint8>(Battlefield | Fished | Called | Event), true, "Battlefield + neutrals can be neutral" },
        { static_cast<uint8>(0x01 | Battlefield), true, "0x01|Battlefield can be neutral" },

        // Combinations with Notorious bit set (blocked).
        { static_cast<uint8>(Notorious | Fished), false, "Notorious|Fished blocked" },
        { static_cast<uint8>(Notorious | Called), false, "Notorious|Called blocked" },
        { static_cast<uint8>(Notorious | Event), false, "Notorious|Event blocked" },
        { static_cast<uint8>(Notorious | 0x01), false, "Notorious|0x01 blocked" },
        { static_cast<uint8>(Notorious | Fished | Called | Event), false, "Notorious + neutrals blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Fished), false, "NM|BF|Fished blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Event), false, "NM|BF|Event blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Fished | Called | Event), false, "all bits blocked" },

        // Full byte edge.
        { 0xFF, false, "0xFF includes Notorious" },
        { 0x2C, true, "Fished|Called|Event (0x2C) can be neutral" }, // 0x04|0x08|0x20
        { 0x12, false, "Notorious|Battlefield (0x12) blocked" },       // 0x02|0x10
    };

    for (const auto& c : cases)
    {
        const bool got     = CanBeNeutral(c.t);
        const bool inlineF = inlineCanBeNeutral(c.t);
        const bool pinF    = pinCanBeNeutral(c.t);
        const bool pure    = !Has(c.t, Notorious);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == !Has(NM)") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Has formula") && ok;
        ok = expect(got == pinF, "dual-wire free == C++ pin formula") && ok;
    }

    // Free == inline == pin across residual poles.
    const uint8 poles[] = {
        Normal,
        Notorious,
        Battlefield,
        static_cast<uint8>(Notorious | Battlefield),
        Fished,
        Called,
        Event,
        0x01,
        static_cast<uint8>(Notorious | Fished),
        static_cast<uint8>(Battlefield | Event),
        static_cast<uint8>(Notorious | Battlefield | Fished | Called | Event),
        0xFF,
        0x00,
    };
    for (const auto t : poles)
    {
        const bool got     = CanBeNeutral(t);
        const bool inlineF = inlineCanBeNeutral(t);
        const bool pinF    = pinCanBeNeutral(t);
        const bool want    = !Has(t, Notorious);
        ok = expect(got == want, "pole free == !Has(NM)") && ok;
        ok = expect(got == inlineF && got == pinF, "pole free == inline == pin") && ok;
    }

    // Dense: single known type bits free == inline == pin.
    const uint8 singleBits[] = {
        Normal, 0x01, Notorious, Fished, Called, Battlefield, Event,
    };
    for (const auto t : singleBits)
    {
        const bool got     = CanBeNeutral(t);
        const bool inlineF = inlineCanBeNeutral(t);
        const bool pinF    = pinCanBeNeutral(t);
        const bool want    = !Has(t, Notorious);
        ok = expect(got == want, "single-bit free == !Has(NM)") && ok;
        ok = expect(got == inlineF && got == pinF, "single-bit free == inline == pin") && ok;
    }

    // Dense compose over full uint8 range: free function matches formula + pin.
    for (unsigned i = 0; i <= 0xFF; ++i)
    {
        const auto t    = static_cast<uint8>(i);
        const bool got  = CanBeNeutral(t);
        const bool want = !Has(t, Notorious);
        ok = expect(got == want, "dense CanBeNeutral formula") && ok;
        ok = expect(got == inlineCanBeNeutral(t), "dense dual-wire == inline") && ok;
        ok = expect(got == pinCanBeNeutral(t), "dense dual-wire == pin") && ok;
    }

    return ok;
}
