#include "test_synthutils_can_synthesize_hq_3794.h"

#include "map/utils/synthutils_capacity.h"

#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "synthutils CanSynthesizeHQ 3794 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline canSynthesizeHQ pure half for dual-wire cross-check
// (slice 3794 dedicated expand residual 2876 / prior ~3749):
//   return getMod(antiHQ) == 0
// Direct return form matching production free function / capacity.
auto inlineCanSynthesizeHQ(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Compact dual-wire pin matching free function / capacity body (slice 3794).
// Direct return only — same formula as production CanSynthesizeHQ.
auto pinCanSynthesizeHQ3794(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3749) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3749(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3704) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3704(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3659) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3659(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3614) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3614(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3569) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3569(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3508) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3508(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3313) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3313(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3283) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3283(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

// Prior dedicated pin (slice 3244) — same formula; retained under 3794.
auto pinCanSynthesizeHQ3244(const int16 antiHQMod) -> bool
{
    return antiHQMod == 0;
}

} // namespace

// Pure dual-wire expansion for synthutilshelpers::CanSynthesizeHQ
// (synthutils::canSynthesizeHQ after skill→Mod mapping / getMod inject;
// OmegaXI internal/synthutils; slice 3794 dedicated expand residual 2876 /
// prior ~3749). Formula unchanged.
//
// Coverage:
//   - free == inline == pin == pin3749 (direct return formula)
//   - residual 2876 / prior dedicated 3749 / 3704 / 3659 / 3614 / 3569 / 3508 / 3313 /
//     3283 / 3244 / pure 1922 poles still hold (0 true, non-zero false)
//   - residual poles + dense range + int16 extremes
auto runSynthutilsCanSynthesizeHQ3794SelfTests() -> bool
{
    using synthutilshelpers::CanSynthesizeHQ;

    bool ok = true;

    // Residual 2876 / 1922 / prior dedicated 3244 / 3283 / 3313 / 3508 / 3569 / 3614 / 3659 / 3704 / 3749.
    ok = expect(CanSynthesizeHQ(0), "residual zero mod allows HQ") && ok;
    ok = expect(!CanSynthesizeHQ(1), "residual positive anti-HQ blocks") && ok;
    ok = expect(!CanSynthesizeHQ(-1), "residual negative anti-HQ blocks") && ok;
    ok = expect(!CanSynthesizeHQ(2), "residual ring-style anti-HQ 2 blocks") && ok;
    ok = expect(!CanSynthesizeHQ(100), "residual large anti-HQ blocks") && ok;
    ok = expect(!CanSynthesizeHQ(-100), "residual large negative anti-HQ blocks") && ok;

    const struct
    {
        int16       antiHQMod;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 2876 / 1922 / prior 3244 / 3283 / 3313 / 3508 / 3569 / 3614 / 3659 / 3704 / 3749 poles.
        { 0, true, "residual zero mod allows HQ" },
        { 1, false, "residual positive anti-HQ blocks" },
        { -1, false, "residual negative anti-HQ blocks" },
        { 2, false, "residual ring-style anti-HQ 2 blocks" },
        { 100, false, "residual large anti-HQ blocks" },
        { -100, false, "residual large negative anti-HQ blocks" },
        { 0, true, "residual 1922 zero pin" },
        { 0, true, "prior 3244 zero pin" },
        { 0, true, "prior 3283 zero pin" },
        { 0, true, "prior 3313 zero pin" },
        { 0, true, "prior 3508 zero pin" },
        { 0, true, "prior 3569 zero pin" },
        { 0, true, "prior 3614 zero pin" },
        { 0, true, "prior 3659 zero pin" },
        { 0, true, "prior 3704 zero pin" },
        { 0, true, "prior 3749 zero pin" },

        // Host inject path poles (canSynthesizeHQ after getMod).
        { 0, true, "host no ring → HQ allowed" },
        { 1, false, "host anti-HQ ring 1 → block" },
        { -1, false, "host defensive negative → block" },
        { 2, false, "host multi-stack anti-HQ → block" },
    };

    for (const auto& p : poles)
    {
        const bool got     = CanSynthesizeHQ(p.antiHQMod);
        const bool inlineF = inlineCanSynthesizeHQ(p.antiHQMod);
        const bool pin     = pinCanSynthesizeHQ3794(p.antiHQMod);
        const bool prior49 = pinCanSynthesizeHQ3749(p.antiHQMod);
        const bool prior04 = pinCanSynthesizeHQ3704(p.antiHQMod);
        const bool prior59 = pinCanSynthesizeHQ3659(p.antiHQMod);
        const bool prior14 = pinCanSynthesizeHQ3614(p.antiHQMod);
        const bool prior69 = pinCanSynthesizeHQ3569(p.antiHQMod);
        const bool prior08 = pinCanSynthesizeHQ3508(p.antiHQMod);
        const bool prior13 = pinCanSynthesizeHQ3313(p.antiHQMod);
        const bool prior83 = pinCanSynthesizeHQ3283(p.antiHQMod);
        const bool prior44 = pinCanSynthesizeHQ3244(p.antiHQMod);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "CanSynthesizeHQ dual-wire == inline getMod==0") && ok;
        ok = expect(got == pin, "CanSynthesizeHQ == pin formula") && ok;
        ok = expect(got == prior49, "CanSynthesizeHQ == prior 3749 pin") && ok;
        ok = expect(got == prior04, "CanSynthesizeHQ == prior 3704 pin") && ok;
        ok = expect(got == prior59, "CanSynthesizeHQ == prior 3659 pin") && ok;
        ok = expect(got == prior14, "CanSynthesizeHQ == prior 3614 pin") && ok;
        ok = expect(got == prior69, "CanSynthesizeHQ == prior 3569 pin") && ok;
        ok = expect(got == prior08, "CanSynthesizeHQ == prior 3508 pin") && ok;
        ok = expect(got == prior13, "CanSynthesizeHQ == prior 3313 pin") && ok;
        ok = expect(got == prior83, "CanSynthesizeHQ == prior 3283 pin") && ok;
        ok = expect(got == prior44, "CanSynthesizeHQ == prior 3244 pin") && ok;
        ok = expect(got == (p.antiHQMod == 0), "CanSynthesizeHQ == formula") && ok;
    }

    // Dense edges: negative, 0, 1, small non-zero, large, int16 extremes.
    // free == inline == pin == pin3749 == formula.
    const struct
    {
        int16       antiHQMod;
        const char* label;
    } edges[] = {
        { -2, "edge neg -2" },
        { -1, "edge neg -1" },
        { 0, "edge zero allows" },
        { 1, "edge one blocks" },
        { 2, "edge two blocks" },
        { 42, "edge mid blocks" },
        { 100, "edge hundred blocks" },
        { -100, "edge -100 blocks" },
        { std::numeric_limits<int16>::max(), "edge int16 max blocks" },
        { std::numeric_limits<int16>::min(), "edge int16 min blocks" },
    };

    for (const auto& e : edges)
    {
        const bool got     = CanSynthesizeHQ(e.antiHQMod);
        const bool inlineF = inlineCanSynthesizeHQ(e.antiHQMod);
        const bool pin     = pinCanSynthesizeHQ3794(e.antiHQMod);
        const bool prior49 = pinCanSynthesizeHQ3749(e.antiHQMod);
        const bool prior04 = pinCanSynthesizeHQ3704(e.antiHQMod);
        const bool prior59 = pinCanSynthesizeHQ3659(e.antiHQMod);
        const bool prior14 = pinCanSynthesizeHQ3614(e.antiHQMod);
        const bool prior69 = pinCanSynthesizeHQ3569(e.antiHQMod);
        const bool prior08 = pinCanSynthesizeHQ3508(e.antiHQMod);
        const bool prior13 = pinCanSynthesizeHQ3313(e.antiHQMod);
        const bool prior83 = pinCanSynthesizeHQ3283(e.antiHQMod);
        const bool prior44 = pinCanSynthesizeHQ3244(e.antiHQMod);
        const bool want    = e.antiHQMod == 0;

        ok = expect(got == want, e.label) && ok;
        ok = expect(got == inlineF, "dense edge free == inline") && ok;
        ok = expect(got == pin, "dense edge free == pin") && ok;
        ok = expect(got == prior49, "dense edge free == prior 3749 pin") && ok;
        ok = expect(got == prior04, "dense edge free == prior 3704 pin") && ok;
        ok = expect(got == prior59, "dense edge free == prior 3659 pin") && ok;
        ok = expect(got == prior14, "dense edge free == prior 3614 pin") && ok;
        ok = expect(got == prior69, "dense edge free == prior 3569 pin") && ok;
        ok = expect(got == prior08, "dense edge free == prior 3508 pin") && ok;
        ok = expect(got == prior13, "dense edge free == prior 3313 pin") && ok;
        ok = expect(got == prior83, "dense edge free == prior 3283 pin") && ok;
        ok = expect(got == prior44, "dense edge free == prior 3244 pin") && ok;
    }

    // Dense compose range identity: antiHQMod -200..200.
    // free == inline == pin == pin3749 == formula.
    for (int16 m = -200; m <= 200; ++m)
    {
        const bool got     = CanSynthesizeHQ(m);
        const bool inlineF = inlineCanSynthesizeHQ(m);
        const bool pin     = pinCanSynthesizeHQ3794(m);
        const bool prior49 = pinCanSynthesizeHQ3749(m);
        const bool prior04 = pinCanSynthesizeHQ3704(m);
        const bool prior59 = pinCanSynthesizeHQ3659(m);
        const bool prior14 = pinCanSynthesizeHQ3614(m);
        const bool prior69 = pinCanSynthesizeHQ3569(m);
        const bool prior08 = pinCanSynthesizeHQ3508(m);
        const bool prior13 = pinCanSynthesizeHQ3313(m);
        const bool prior83 = pinCanSynthesizeHQ3283(m);
        const bool prior44 = pinCanSynthesizeHQ3244(m);
        const bool want    = m == 0;

        ok = expect(got == want, "dense compose free == formula") && ok;
        ok = expect(got == inlineF, "dense compose free == inline") && ok;
        ok = expect(got == pin, "dense compose free == pin") && ok;
        ok = expect(got == prior49, "dense compose free == prior 3749 pin") && ok;
        ok = expect(got == prior04, "dense compose free == prior 3704 pin") && ok;
        ok = expect(got == prior14, "dense compose free == prior 3614 pin") && ok;
        ok = expect(got == prior59, "dense compose free == prior 3659 pin") && ok;
        ok = expect(got == prior69, "dense compose free == prior 3569 pin") && ok;
        ok = expect(got == prior08, "dense compose free == prior 3508 pin") && ok;
        ok = expect(got == prior13, "dense compose free == prior 3313 pin") && ok;
        ok = expect(got == prior83, "dense compose free == prior 3283 pin") && ok;
        ok = expect(got == prior44, "dense compose free == prior 3244 pin") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(CanSynthesizeHQ(0), "antiHQMod 0 must allow HQ") && ok;
    ok = expect(!CanSynthesizeHQ(1), "antiHQMod 1 must block") && ok;
    ok = expect(!CanSynthesizeHQ(-1), "antiHQMod -1 must block") && ok;
    ok = expect(!CanSynthesizeHQ(std::numeric_limits<int16>::max()), "antiHQMod int16 max must block") && ok;

    // Production path semantics (host inject model for canSynthesizeHQ).
    const struct
    {
        int16       antiHQMod;
        const char* label;
    } hostPoles[] = {
        { 0, "eligible HQ path" },
        { 1, "anti-HQ ring blocks" },
        { 2, "multi-stack anti-HQ blocks" },
        { -1, "defensive negative blocks" },
        { 100, "large anti-HQ blocks" },
    };

    for (const auto& p : hostPoles)
    {
        const bool got     = CanSynthesizeHQ(p.antiHQMod);
        const bool inlineF = inlineCanSynthesizeHQ(p.antiHQMod);
        const bool pin     = pinCanSynthesizeHQ3794(p.antiHQMod);
        const bool prior49 = pinCanSynthesizeHQ3749(p.antiHQMod);
        const bool prior04 = pinCanSynthesizeHQ3704(p.antiHQMod);
        const bool prior59 = pinCanSynthesizeHQ3659(p.antiHQMod);
        const bool prior14 = pinCanSynthesizeHQ3614(p.antiHQMod);
        const bool prior69 = pinCanSynthesizeHQ3569(p.antiHQMod);
        const bool prior08 = pinCanSynthesizeHQ3508(p.antiHQMod);
        const bool prior13 = pinCanSynthesizeHQ3313(p.antiHQMod);
        const bool prior83 = pinCanSynthesizeHQ3283(p.antiHQMod);
        const bool prior44 = pinCanSynthesizeHQ3244(p.antiHQMod);

        ok = expect(got == pin, p.label) && ok;
        ok = expect(got == inlineF, "host pole free == inline") && ok;
        ok = expect(got == prior49, "host pole free == prior 3749 pin") && ok;
        ok = expect(got == prior04, "host pole free == prior 3704 pin") && ok;
        ok = expect(got == prior59, "host pole free == prior 3659 pin") && ok;
        ok = expect(got == prior14, "host pole free == prior 3614 pin") && ok;
        ok = expect(got == prior69, "host pole free == prior 3569 pin") && ok;
        ok = expect(got == prior08, "host pole free == prior 3508 pin") && ok;
        ok = expect(got == prior13, "host pole free == prior 3313 pin") && ok;
        ok = expect(got == prior83, "host pole free == prior 3283 pin") && ok;
        ok = expect(got == prior44, "host pole free == prior 3244 pin") && ok;
    }

    return ok;
}
