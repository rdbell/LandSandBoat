#include "test_mobtype_can_deaggro_3338.h"

#include "map/mobtype_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobtype CanDeaggro 3338 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CMobEntity::CanDeaggro type policy for dual-wire cross-check
// (dedicated slice 3338 expand residual 2919 / prior dedicated 3063):
//   !Has(t, Notorious) && !Has(t, Battlefield)
// Positive form only (no De Morgan rewrite).
auto inlineCanDeaggro(const uint8 t) -> bool
{
    return !mobtypehelpers::Has(t, mobtypehelpers::Notorious) &&
           !mobtypehelpers::Has(t, mobtypehelpers::Battlefield);
}

// Compact dual-wire pin matching Go pinCanDeaggro3338 / C++ capacity
// direct form (prefer positive !Has && !Has over bool-split / De Morgan):
//   !Has(t, Notorious) && !Has(t, Battlefield)
auto pinCanDeaggro(const uint8 t) -> bool
{
    return !mobtypehelpers::Has(t, mobtypehelpers::Notorious) &&
           !mobtypehelpers::Has(t, mobtypehelpers::Battlefield);
}

} // namespace

// Pure dual-wire expansion for mobtypehelpers::CanDeaggro
// (CMobEntity::CanDeaggro type-bit gate; OmegaXI internal/mobtype;
// dedicated slice 3338 expand residual 2919 / prior dedicated 3063 /
// pure 2042/2655).
//
// Coverage:
//   - free == inline == pin == !Has(NM) && !Has(BF) (positive form only)
//   - residual poles: Normal, Notorious, Battlefield, NM|BF
//   - dense: zero type, all single known type bits, gate combinations
// Formula unchanged — not registered in CMake/main.
auto runMobtypeCanDeaggro3338SelfTests() -> bool
{
    using mobtypehelpers::Battlefield;
    using mobtypehelpers::Called;
    using mobtypehelpers::CanDeaggro;
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

    // Residual 2042 / 2655 / 2919 / 3063 pins still hold under dual-wire.
    ok = expect(CanDeaggro(Normal), "residual: no flags → true") && ok;
    ok = expect(!CanDeaggro(Notorious), "residual: Notorious only → false") && ok;
    ok = expect(!CanDeaggro(Battlefield), "residual: Battlefield only → false") && ok;
    ok = expect(!CanDeaggro(static_cast<uint8>(Notorious | Battlefield)), "residual: both → false") && ok;

    // Other flags alone still true if neither Notorious nor Battlefield.
    ok = expect(CanDeaggro(Fished), "Fished alone deaggroes") && ok;
    ok = expect(CanDeaggro(Called), "Called alone deaggroes") && ok;
    ok = expect(CanDeaggro(Event), "Event alone deaggroes") && ok;
    ok = expect(CanDeaggro(0x01), "MOBTYPE_0X01 reserve deaggroes") && ok;
    ok = expect(CanDeaggro(static_cast<uint8>(Fished | Called | Event)), "all neutral bits deaggro") && ok;

    const struct
    {
        uint8       t;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual poles (slice 2919 / 3063 / 3338).
        { Normal, true, "no flags → true" },
        { 0x00, true, "explicit zero → true" },
        { Notorious, false, "Notorious only → false" },
        { Battlefield, false, "Battlefield only → false" },
        { static_cast<uint8>(Notorious | Battlefield), false, "both NM|BF → false" },

        // All single known type bits.
        { Fished, true, "Fished alone deaggroes" },
        { Called, true, "Called alone deaggroes" },
        { Event, true, "Event alone deaggroes" },
        { 0x01, true, "MOBTYPE_0X01 reserve deaggroes" },

        // Combinations with neither gate set.
        { static_cast<uint8>(Fished | Called), true, "Fished|Called deaggroes" },
        { static_cast<uint8>(Fished | Event), true, "Fished|Event deaggroes" },
        { static_cast<uint8>(Called | Event), true, "Called|Event deaggroes" },
        { static_cast<uint8>(Fished | Called | Event), true, "all neutral bits deaggro" },
        { static_cast<uint8>(0x01 | Fished), true, "0x01|Fished deaggroes" },
        { static_cast<uint8>(0x01 | Called | Event), true, "0x01|Called|Event deaggroes" },

        // Notorious with other flags still blocked.
        { static_cast<uint8>(Notorious | Fished), false, "Notorious|Fished blocked" },
        { static_cast<uint8>(Notorious | Called), false, "Notorious|Called blocked" },
        { static_cast<uint8>(Notorious | Event), false, "Notorious|Event blocked" },
        { static_cast<uint8>(Notorious | 0x01), false, "Notorious|0x01 blocked" },
        { static_cast<uint8>(Notorious | Fished | Called | Event), false, "Notorious + neutrals blocked" },

        // Battlefield with other flags still blocked.
        { static_cast<uint8>(Battlefield | Fished), false, "Battlefield|Fished blocked" },
        { static_cast<uint8>(Battlefield | Called), false, "Battlefield|Called blocked" },
        { static_cast<uint8>(Battlefield | Event), false, "Battlefield|Event blocked" },
        { static_cast<uint8>(Battlefield | 0x01), false, "Battlefield|0x01 blocked" },
        { static_cast<uint8>(Battlefield | Fished | Called | Event), false, "Battlefield + neutrals blocked" },

        // Combined notorious + battlefield + neutrals blocked.
        { static_cast<uint8>(Notorious | Battlefield | Fished), false, "NM|BF|Fished blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Event), false, "NM|BF|Event blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Fished | Called | Event), false, "all bits blocked" },

        // Full byte edge.
        { 0xFF, false, "0xFF includes both blocking bits" },
        { 0x2C, true, "Fished|Called|Event (0x2C) deaggroes" }, // 0x04|0x08|0x20
        { 0x12, false, "Notorious|Battlefield (0x12) blocked" },  // 0x02|0x10
    };

    for (const auto& c : cases)
    {
        const bool got     = CanDeaggro(c.t);
        const bool inlineF = inlineCanDeaggro(c.t);
        const bool pinF    = pinCanDeaggro(c.t);
        const bool pure    = !Has(c.t, Notorious) && !Has(c.t, Battlefield);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == !Has(NM) && !Has(BF)") && ok;
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
        const bool got     = CanDeaggro(t);
        const bool inlineF = inlineCanDeaggro(t);
        const bool pinF    = pinCanDeaggro(t);
        const bool want    = !Has(t, Notorious) && !Has(t, Battlefield);
        ok = expect(got == want, "pole free == !Has(NM) && !Has(BF)") && ok;
        ok = expect(got == inlineF && got == pinF, "pole free == inline == pin") && ok;
    }

    // Dense: single known type bits free == inline == pin.
    const uint8 singleBits[] = {
        Normal, 0x01, Notorious, Fished, Called, Battlefield, Event,
    };
    for (const auto t : singleBits)
    {
        const bool got     = CanDeaggro(t);
        const bool inlineF = inlineCanDeaggro(t);
        const bool pinF    = pinCanDeaggro(t);
        const bool want    = !Has(t, Notorious) && !Has(t, Battlefield);
        ok = expect(got == want, "single-bit free == !Has(NM) && !Has(BF)") && ok;
        ok = expect(got == inlineF && got == pinF, "single-bit free == inline == pin") && ok;
    }

    // Dense compose over full uint8 range: free function matches formula + pin.
    for (unsigned i = 0; i <= 0xFF; ++i)
    {
        const auto t    = static_cast<uint8>(i);
        const bool got  = CanDeaggro(t);
        const bool want = !Has(t, Notorious) && !Has(t, Battlefield);
        ok = expect(got == want, "dense CanDeaggro formula") && ok;
        ok = expect(got == inlineCanDeaggro(t), "dense dual-wire == inline") && ok;
        ok = expect(got == pinCanDeaggro(t), "dense dual-wire == pin") && ok;
    }

    return ok;
}
