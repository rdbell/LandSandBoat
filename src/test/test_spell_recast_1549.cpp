#include "test_spell_recast_1549.h"

#include "map/spell_recast_capacity.h"

#include <iostream>

namespace
{
using spellrecasthelpers::CalculateSpellRecastMs;
using spellrecasthelpers::FloorMulMs;
using spellrecasthelpers::JobSCH;
using spellrecasthelpers::RecastCapFloorMs;
using spellrecasthelpers::ShouldReturnZeroNullSpell;
using spellrecasthelpers::SpellGroupBlack;
using spellrecasthelpers::SpellGroupSong;
using spellrecasthelpers::SpellGroupWhite;
using spellrecasthelpers::SpellRecastParams;

auto BaseParams(const std::int64_t baseMs) -> SpellRecastParams
{
    SpellRecastParams p{};
    p.baseMs             = baseMs;
    p.recastReductionCap = 80.0f;
    return p;
}

auto Check() -> bool
{
    auto fail = [](const char* label, const std::int64_t got, const std::int64_t want) {
        std::cerr << "spell_recast_1549 fail: " << label << " got=" << got << " want=" << want << '\n';
        return false;
    };

    if (!ShouldReturnZeroNullSpell(true) || ShouldReturnZeroNullSpell(false))
    {
        return fail("null-spell-gate", 0, 0);
    }
    if (FloorMulMs(1000, 0.5f) != 500)
    {
        return fail("floor-half", FloorMulMs(1000, 0.5f), 500);
    }
    if (FloorMulMs(1000, 0.333f) != 333 || FloorMulMs(-1000, 0.5f) != -500)
    {
        return fail("floor-boundaries", FloorMulMs(1000, 0.333f), 333);
    }
    // cap 80% → floor base*0.2
    if (RecastCapFloorMs(10000, 80.0f) != 2000 || RecastCapFloorMs(10000, 0.0f) != 10000 || RecastCapFloorMs(10000, 100.0f) != 0)
    {
        return fail("cap-floor", RecastCapFloorMs(10000, 80.0f), 2000);
    }

    // No mods: base unchanged then max with floor(base*0.2) still base
    {
        auto p = BaseParams(10000);
        const auto got = CalculateSpellRecastMs(p);
        if (got != 10000)
        {
            return fail("base-passthrough", got, 10000);
        }
    }

    // Fast Cast 80 → 40% recast reduction (float floor may not be exact 6000)
    {
        auto p     = BaseParams(10000);
        p.fastCast = 80;
        const auto want = FloorMulMs(10000, (100.0f - 40.0f) / 100.0f);
        const auto got  = CalculateSpellRecastMs(p);
        if (got != want)
        {
            return fail("fast-cast", got, want);
        }
    }

    // Nightingale halves song
    {
        auto p        = BaseParams(10000);
        p.spellGroup  = SpellGroupSong;
        p.nightingale = true;
        const auto want = FloorMulMs(10000, 0.5f);
        // song then max with floor still
        const auto got = CalculateSpellRecastMs(p);
        if (got != want)
        {
            return fail("nightingale", got, want);
        }
    }

    // Black Manifestation non-SCH ×3
    {
        auto p            = BaseParams(10000);
        p.spellGroup      = SpellGroupBlack;
        p.aoe             = spellrecasthelpers::AOERadialMani;
        p.manifestation   = true;
        p.mainJob         = 1; // not SCH
        const auto got = CalculateSpellRecastMs(p);
        if (got != 30000)
        {
            return fail("mani-non-sch", got, 30000);
        }
    }

    // Black Manifestation SCH ×2
    {
        auto p          = BaseParams(10000);
        p.spellGroup    = SpellGroupBlack;
        p.aoe           = spellrecasthelpers::AOERadialMani;
        p.manifestation = true;
        p.mainJob       = JobSCH;
        const auto got = CalculateSpellRecastMs(p);
        if (got != 20000)
        {
            return fail("mani-sch", got, 20000);
        }
    }

    // Alacrity 0.60 after base path (use FloorMulMs for expected)
    {
        auto p       = BaseParams(10000);
        p.spellGroup = SpellGroupBlack;
        p.alacrity   = true;
        // black else branch: FloorMulMs(10000, (100+0)/100) = 10000, max floor, then *0.60
        const auto want = FloorMulMs(10000, 0.60f);
        const auto got  = CalculateSpellRecastMs(p);
        if (got != want)
        {
            return fail("alacrity", got, want);
        }
    }

    // White Celerity + weather bonus 20 → *0.8 after *0.60
    {
        auto p                      = BaseParams(10000);
        p.spellGroup                = SpellGroupWhite;
        p.celerity                  = true;
        p.weatherMatchesElement     = true;
        p.alacrityCelerityEffect    = 20;
        // white else: FloorMulMs(10000, 1.0) = 10000
        const auto afterAlac = FloorMulMs(10000, 0.60f);
        const auto want      = FloorMulMs(afterAlac, (100.0f - 20.0f) / 100.0f);
        const auto got       = CalculateSpellRecastMs(p);
        if (got != want)
        {
            return fail("celerity-weather", got, want);
        }
    }

    // Composure 1.25
    {
        auto p      = BaseParams(8000);
        p.composure = true;
        const auto want = FloorMulMs(8000, 1.25f);
        const auto got  = CalculateSpellRecastMs(p);
        if (got != want)
        {
            return fail("composure", got, want);
        }
    }

    // Huge song delay: subtract goes negative, then recastCapFloor lifts to 20% of base
    // (LSB applies max(recast, floor) before final max(0); final is still the floor).
    {
        auto p                 = BaseParams(1000);
        p.spellGroup           = SpellGroupSong;
        p.songRecastDelaySec   = 100;
        const auto want        = RecastCapFloorMs(1000, 80.0f); // 200
        const auto got         = CalculateSpellRecastMs(p);
        if (got != want)
        {
            return fail("song-delay-floor", got, want);
        }
    }

    return true;
}
} // namespace

auto runSpellRecast1549SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "spell_recast_1549 self-tests failed\n";
        return false;
    }
    return true;
}
