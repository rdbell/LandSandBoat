#include "test_synthutils_can_hq_2876.h"

#include "map/utils/synthutils_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synthutils can HQ 2876 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline canSynthesizeHQ pure half for dual-wire checks:
//   return getMod(antiHQ) == 0
auto inlineCanSynthesizeHQ(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

} // namespace

// Pure dual-wire expansion for synthutilshelpers::CanSynthesizeHQ
// (synthutils::canSynthesizeHQ after skill→Mod mapping / getMod inject).
auto runSynthutilsCanHQ2876SelfTests() -> bool
{
    using synthutilshelpers::CanSynthesizeHQ;

    bool ok = true;

    const struct
    {
        int16       antiHQMod;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, true, "zero mod allows HQ" },
        { 1, false, "positive anti-HQ blocks" },
        { -1, false, "negative anti-HQ blocks" },
        { 2, false, "ring-style anti-HQ 2 blocks" },
        { 100, false, "large anti-HQ blocks" },
        { -100, false, "large negative anti-HQ blocks" },
        { 0, true, "residual 1922 zero pin" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanSynthesizeHQ(c.antiHQMod);
        const bool inlineF = inlineCanSynthesizeHQ(c.antiHQMod);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline getMod==0") && ok;
        ok = expect(got == (c.antiHQMod == 0), "dual-wire free == antiHQMod==0") && ok;
    }

    // Residual 1922 pins (zero allows; any non-zero blocks).
    ok = expect(CanSynthesizeHQ(0), "residual CanSynthesizeHQ(0)") && ok;
    ok = expect(!CanSynthesizeHQ(1), "residual CanSynthesizeHQ(1)") && ok;
    ok = expect(!CanSynthesizeHQ(-1), "residual CanSynthesizeHQ(-1)") && ok;

    // Dense compose range identity: free function matches antiHQMod == 0.
    for (int16 m = -200; m <= 200; ++m)
    {
        const bool got  = CanSynthesizeHQ(m);
        const bool want = m == 0;
        ok = expect(got == want, "compose range free == antiHQMod==0") && ok;
        ok = expect(got == inlineCanSynthesizeHQ(m), "compose range free == inline") && ok;
    }

    return ok;
}
