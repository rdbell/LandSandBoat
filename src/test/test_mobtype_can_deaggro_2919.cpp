#include "test_mobtype_can_deaggro_2919.h"

#include "map/mobtype_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobtype CanDeaggro 2919 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CMobEntity::CanDeaggro type policy for dual-wire cross-check:
//   !(t & MOBTYPE_NOTORIOUS) && !(t & MOBTYPE_BATTLEFIELD)
// which is equivalent to:
//   !Has(t, Notorious) && !Has(t, Battlefield)
auto inlineCanDeaggro(const uint8 t) -> bool
{
    return !mobtypehelpers::Has(t, mobtypehelpers::Notorious) &&
           !mobtypehelpers::Has(t, mobtypehelpers::Battlefield);
}

} // namespace

// Pure dual-wire expansion for mobtypehelpers::CanDeaggro
// (CMobEntity::CanDeaggro type-bit gate; OmegaXI internal/mobtype).
auto runMobtypeCanDeaggro2919SelfTests() -> bool
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

    const struct
    {
        uint8       t;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2042 / 2655 pins (Type form).
        { Normal, true, "residual Normal deaggroes" },
        { Notorious, false, "residual Notorious does not deaggro" },
        { Battlefield, false, "residual Battlefield does not deaggro" },
        { static_cast<uint8>(Notorious | Battlefield), false, "residual combined does not deaggro" },
        // Neutral type bits still deaggro.
        { Fished, true, "Fished alone deaggroes" },
        { Called, true, "Called alone deaggroes" },
        { Event, true, "Event alone deaggroes" },
        { static_cast<uint8>(Fished | Called), true, "Fished|Called deaggroes" },
        { static_cast<uint8>(Fished | Event), true, "Fished|Event deaggroes" },
        { static_cast<uint8>(Called | Event), true, "Called|Event deaggroes" },
        { static_cast<uint8>(Fished | Called | Event), true, "all neutral bits deaggro" },
        // Notorious with neutral bits still blocked.
        { static_cast<uint8>(Notorious | Fished), false, "Notorious|Fished blocked" },
        { static_cast<uint8>(Notorious | Called), false, "Notorious|Called blocked" },
        { static_cast<uint8>(Notorious | Event), false, "Notorious|Event blocked" },
        { static_cast<uint8>(Notorious | Fished | Called | Event), false, "Notorious + neutrals blocked" },
        // Battlefield with neutral bits still blocked.
        { static_cast<uint8>(Battlefield | Fished), false, "Battlefield|Fished blocked" },
        { static_cast<uint8>(Battlefield | Called), false, "Battlefield|Called blocked" },
        { static_cast<uint8>(Battlefield | Event), false, "Battlefield|Event blocked" },
        { static_cast<uint8>(Battlefield | Fished | Called | Event), false, "Battlefield + neutrals blocked" },
        // Combined notorious + battlefield + neutrals blocked.
        { static_cast<uint8>(Notorious | Battlefield | Fished), false, "NM|BF|Fished blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Event), false, "NM|BF|Event blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Fished | Called | Event), false, "all bits blocked" },
        // Available01 reserve bit is neutral.
        { 0x01, true, "MOBTYPE_0X01 reserve deaggroes" },
        { static_cast<uint8>(0x01 | Fished), true, "0x01|Fished deaggroes" },
        { static_cast<uint8>(0x01 | Notorious), false, "0x01|Notorious blocked" },
        { static_cast<uint8>(0x01 | Battlefield), false, "0x01|Battlefield blocked" },
        // Full byte edge.
        { 0xFF, false, "0xFF includes both blocking bits" },
        { 0x00, true, "explicit zero deaggroes" },
        { 0x2C, true, "Fished|Called|Event (0x2C) deaggroes" }, // 0x04|0x08|0x20
        { 0x12, false, "Notorious|Battlefield (0x12) blocked" },  // 0x02|0x10
    };

    for (const auto& c : cases)
    {
        const bool got     = CanDeaggro(c.t);
        const bool inlineF = inlineCanDeaggro(c.t);
        const bool pure    = !Has(c.t, Notorious) && !Has(c.t, Battlefield);
        // Bool-split form used by production mobbehaviorhelpers host extract.
        const bool boolForm = !static_cast<bool>(c.t & Notorious) && !static_cast<bool>(c.t & Battlefield);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == !Has(NM) && !Has(BF)") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Has formula") && ok;
        ok = expect(got == boolForm, "dual-wire free == bool-split host form") && ok;
    }

    // Residual 2042 / 2655 explicit pins.
    ok = expect(CanDeaggro(Normal), "residual CanDeaggro(Normal)") && ok;
    ok = expect(!CanDeaggro(Notorious), "residual CanDeaggro(Notorious)") && ok;
    ok = expect(!CanDeaggro(Battlefield), "residual CanDeaggro(Battlefield)") && ok;
    ok = expect(!CanDeaggro(static_cast<uint8>(Notorious | Battlefield)), "residual CanDeaggro(NM|BF)") && ok;

    // Dense compose over full uint8 range: free function matches formula.
    for (unsigned i = 0; i <= 0xFF; ++i)
    {
        const auto t    = static_cast<uint8>(i);
        const bool got  = CanDeaggro(t);
        const bool want = !Has(t, Notorious) && !Has(t, Battlefield);
        ok = expect(got == want, "dense CanDeaggro formula") && ok;
        ok = expect(got == inlineCanDeaggro(t), "dense dual-wire == inline") && ok;
    }

    return ok;
}
