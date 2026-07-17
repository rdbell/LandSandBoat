#include "test_mobtype_can_be_neutral_2934.h"

#include "map/mobtype_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobtype CanBeNeutral 2934 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CMobEntity::CanBeNeutral type policy for dual-wire cross-check:
//   !(t & MOBTYPE_NOTORIOUS)
// which is equivalent to:
//   !Has(t, Notorious)
auto inlineCanBeNeutral(const uint8 t) -> bool
{
    return !mobtypehelpers::Has(t, mobtypehelpers::Notorious);
}

} // namespace

// Pure dual-wire expansion for mobtypehelpers::CanBeNeutral
// (CMobEntity::CanBeNeutral type-bit gate; OmegaXI internal/mobtype).
auto runMobtypeCanBeNeutral2934SelfTests() -> bool
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

    const struct
    {
        uint8       t;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2042 / 2655 pins (Type form).
        { Normal, true, "residual Normal can be neutral" },
        { Notorious, false, "residual Notorious cannot be neutral" },
        // Battlefield alone is not notorious — may be neutral.
        { Battlefield, true, "Battlefield alone can be neutral" },
        { static_cast<uint8>(Notorious | Battlefield), false, "residual combined NM|BF cannot be neutral" },
        // Neutral type bits still allow neutral.
        { Fished, true, "Fished alone can be neutral" },
        { Called, true, "Called alone can be neutral" },
        { Event, true, "Event alone can be neutral" },
        { static_cast<uint8>(Fished | Called), true, "Fished|Called can be neutral" },
        { static_cast<uint8>(Fished | Event), true, "Fished|Event can be neutral" },
        { static_cast<uint8>(Called | Event), true, "Called|Event can be neutral" },
        { static_cast<uint8>(Fished | Called | Event), true, "all neutral bits can be neutral" },
        // Notorious with neutral bits still blocked.
        { static_cast<uint8>(Notorious | Fished), false, "Notorious|Fished blocked" },
        { static_cast<uint8>(Notorious | Called), false, "Notorious|Called blocked" },
        { static_cast<uint8>(Notorious | Event), false, "Notorious|Event blocked" },
        { static_cast<uint8>(Notorious | Fished | Called | Event), false, "Notorious + neutrals blocked" },
        // Battlefield with neutral bits still allow neutral (NM is the only gate).
        { static_cast<uint8>(Battlefield | Fished), true, "Battlefield|Fished can be neutral" },
        { static_cast<uint8>(Battlefield | Called), true, "Battlefield|Called can be neutral" },
        { static_cast<uint8>(Battlefield | Event), true, "Battlefield|Event can be neutral" },
        { static_cast<uint8>(Battlefield | Fished | Called | Event), true, "Battlefield + neutrals can be neutral" },
        // Combined notorious + battlefield + neutrals blocked by NM.
        { static_cast<uint8>(Notorious | Battlefield | Fished), false, "NM|BF|Fished blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Event), false, "NM|BF|Event blocked" },
        { static_cast<uint8>(Notorious | Battlefield | Fished | Called | Event), false, "all bits blocked" },
        // Available01 reserve bit is neutral.
        { 0x01, true, "MOBTYPE_0X01 reserve can be neutral" },
        { static_cast<uint8>(0x01 | Fished), true, "0x01|Fished can be neutral" },
        { static_cast<uint8>(0x01 | Notorious), false, "0x01|Notorious blocked" },
        { static_cast<uint8>(0x01 | Battlefield), true, "0x01|Battlefield can be neutral" },
        // Full byte edge.
        { 0xFF, false, "0xFF includes Notorious" },
        { 0x00, true, "explicit zero can be neutral" },
        { 0x2C, true, "Fished|Called|Event (0x2C) can be neutral" }, // 0x04|0x08|0x20
        { 0x12, false, "Notorious|Battlefield (0x12) blocked" },       // 0x02|0x10
    };

    for (const auto& c : cases)
    {
        const bool got     = CanBeNeutral(c.t);
        const bool inlineF = inlineCanBeNeutral(c.t);
        const bool pure    = !Has(c.t, Notorious);
        // Bool-split form used by production mobbehaviorhelpers host extract.
        const bool boolForm = !static_cast<bool>(c.t & Notorious);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pure, "dual-wire free == !Has(NM)") && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Has formula") && ok;
        ok = expect(got == boolForm, "dual-wire free == bool-split host form") && ok;
    }

    // Residual 2042 / 2655 explicit pins.
    ok = expect(CanBeNeutral(Normal), "residual CanBeNeutral(Normal)") && ok;
    ok = expect(!CanBeNeutral(Notorious), "residual CanBeNeutral(Notorious)") && ok;
    ok = expect(CanBeNeutral(Battlefield), "residual CanBeNeutral(Battlefield)") && ok;
    ok = expect(!CanBeNeutral(static_cast<uint8>(Notorious | Battlefield)), "residual CanBeNeutral(NM|BF)") && ok;

    // Dense compose over full uint8 range: free function matches formula.
    for (unsigned i = 0; i <= 0xFF; ++i)
    {
        const auto t    = static_cast<uint8>(i);
        const bool got  = CanBeNeutral(t);
        const bool want = !Has(t, Notorious);
        ok = expect(got == want, "dense CanBeNeutral formula") && ok;
        ok = expect(got == inlineCanBeNeutral(t), "dense dual-wire == inline") && ok;
    }

    return ok;
}
