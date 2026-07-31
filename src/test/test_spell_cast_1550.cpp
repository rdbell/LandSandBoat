#include "test_spell_cast_1550.h"

#include "map/spell_cast_capacity.h"

#include <iostream>

namespace
{
using spellcasthelpers::CalculateSpellCastMs;
using spellcasthelpers::ClampI16;
using spellcasthelpers::FloorMulMs;
using spellcasthelpers::ShouldReturnZeroNullSpell;
using spellcasthelpers::SpellCastParams;
using spellcasthelpers::SpellGroupBlack;
using spellcasthelpers::SpellGroupNinjutsu;
using spellcasthelpers::SpellGroupSong;
using spellcasthelpers::SpellGroupSummoning;
using spellcasthelpers::SpellGroupWhite;

auto Base(const std::int64_t baseMs) -> SpellCastParams
{
    SpellCastParams p{};
    p.baseMs = baseMs;
    return p;
}

auto Check() -> bool
{
    auto fail = [](const char* label, const std::int64_t got, const std::int64_t want) {
        std::cerr << "spell_cast_1550 fail: " << label << " got=" << got << " want=" << want << '\n';
        return false;
    };

    if (!ShouldReturnZeroNullSpell(true) || ShouldReturnZeroNullSpell(false))
    {
        return fail("null", 0, 0);
    }
    if (ClampI16(-120, -100, 50) != -100 || ClampI16(-100, -100, 50) != -100 ||
        ClampI16(25, -100, 50) != 25 || ClampI16(50, -100, 50) != 50 || ClampI16(75, -100, 50) != 50)
    {
        return fail("clamp-i16", ClampI16(75, -100, 50), 50);
    }
    {
        auto p = Base(5000);
        p.quickMagicProc = true;
        if (CalculateSpellCastMs(p) != 0)
        {
            return fail("quick-magic", CalculateSpellCastMs(p), 0);
        }
    }
    {
        auto p = Base(10000);
        if (CalculateSpellCastMs(p) != 10000)
        {
            return fail("base", CalculateSpellCastMs(p), 10000);
        }
    }
    {
        auto p          = Base(10000);
        p.hassoOrSeigan = true;
        const auto want = FloorMulMs(10000, 1.5f);
        const auto got  = CalculateSpellCastMs(p);
        if (got != want)
        {
            return fail("hasso", got, want);
        }
    }
    // Alacrity base 50% reduction of base from cast: cast = base - floor(base*0.5)
    {
        auto p       = Base(10000);
        p.spellGroup = SpellGroupBlack;
        p.alacrity   = true;
        const auto want = 10000 - FloorMulMs(10000, 0.5f);
        const auto got  = CalculateSpellCastMs(p);
        if (got != want)
        {
            return fail("alacrity", got, want);
        }
    }
    // Black arts +10%
    {
        auto p             = Base(10000);
        p.spellGroup       = SpellGroupBlack;
        p.blackMagicCast   = 10;
        const auto want    = FloorMulMs(10000, 1.10f);
        const auto got     = CalculateSpellCastMs(p);
        if (got != want)
        {
            return fail("black-arts", got, want);
        }
    }
    // Summoning flat 2s
    {
        auto p                 = Base(5000);
        p.spellGroup           = SpellGroupSummoning;
        p.summoningMagicCast   = 2;
        if (CalculateSpellCastMs(p) != 3000)
        {
            return fail("summon", CalculateSpellCastMs(p), 3000);
        }
    }
    // Song nightingale half
    {
        auto p          = Base(8000);
        p.spellGroup    = SpellGroupSong;
        p.nightingale   = true;
        const auto want = FloorMulMs(8000, 0.5f);
        const auto got  = CalculateSpellCastMs(p);
        if (got != want)
        {
            return fail("nightingale", got, want);
        }
    }
    // Nightingale instant
    {
        auto p                   = Base(8000);
        p.spellGroup             = SpellGroupSong;
        p.nightingale            = true;
        p.isPC                   = true;
        p.nightingaleInstant     = true;
        if (CalculateSpellCastMs(p) != 0)
        {
            return fail("nightingale-instant", CalculateSpellCastMs(p), 0);
        }
    }
    // Ninjutsu JP 2 → 6% reduction
    {
        auto p                    = Base(10000);
        p.spellGroup              = SpellGroupNinjutsu;
        p.isPC                    = true;
        p.ninjutsuCastTimeBonus   = 2;
        const auto want           = FloorMulMs(10000, 1.0f - 0.03f * 2);
        const auto got            = CalculateSpellCastMs(p);
        if (got != want)
        {
            return fail("ninjutsu", got, want);
        }
    }
    // Fast Cast 50 final factor (100-50)/100
    {
        auto p       = Base(10000);
        p.fastCast   = 50;
        const auto want = FloorMulMs(10000, 0.50f);
        const auto got  = CalculateSpellCastMs(p);
        if (got != want)
        {
            return fail("fastcast", got, want);
        }
    }
    // White Celerity with weather+JP bonus
    {
        auto p                   = Base(10000);
        p.spellGroup             = SpellGroupWhite;
        p.celerity               = true;
        p.isPC                   = true;
        p.weatherMatchesElement  = true;
        p.alacrityCelerityEffect = 10;
        p.strategemEffectII      = 5;
        // bonus=15, reduction = (100-(50+15))/100 = 0.35; cast = 10000 - floor(10000*0.35)
        const auto want = 10000 - FloorMulMs(10000, 0.35f);
        const auto got  = CalculateSpellCastMs(p);
        if (got != want)
        {
            return fail("celerity-bonus", got, want);
        }
    }
    return true;
}
} // namespace

auto runSpellCast1550SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "spell_cast_1550 self-tests failed\n";
        return false;
    }
    return true;
}
