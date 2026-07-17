#include "test_chocobo_number_to_rank_2862.h"

#include "map/chocobo_raising_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "chocobo number to rank 2862 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline intended upper-bound formula for dual-wire cross-checks
// (not the stock Lua ipairs no-op over string-keyed skillRankBoundaries).
auto inlineNumberToRank(const int32 skill) -> uint8
{
    if (skill < 0)
    {
        return 0;
    }
    constexpr int32 boundaries[8] = { 31, 63, 95, 127, 159, 191, 223, 255 };
    for (uint8 rank = 0; rank < 8; ++rank)
    {
        if (skill <= boundaries[rank])
        {
            return rank;
        }
    }
    return 7;
}

} // namespace

// Pure dual-wire expansion for chocoboraisinghelpers::NumberToRank
// (intended upper-bound skill→rank bands; slice 2862).
auto runChocoboNumberToRank2862SelfTests() -> bool
{
    using chocoboraisinghelpers::AffectionToAffectionRank;
    using chocoboraisinghelpers::NumberToRank;
    using chocoboraisinghelpers::kSkillRankBoundaries;

    bool ok = true;

    // Boundary pin table.
    constexpr int32 wantBounds[8] = { 31, 63, 95, 127, 159, 191, 223, 255 };
    for (int i = 0; i < 8; ++i)
    {
        ok = expect(kSkillRankBoundaries[i] == wantBounds[i], "boundary pin") && ok;
    }

    // Edge and band cases.
    const struct
    {
        int32       skill;
        uint8       want;
        const char* label;
    } cases[] = {
        { -1, 0, "negative → F" },
        { 0, 0, "0 → F" },
        { 1, 0, "1 → F" },
        { 31, 0, "31 → F" },
        { 32, 1, "32 → E" },
        { 63, 1, "63 → E" },
        { 64, 2, "64 → D" },
        { 95, 2, "95 → D" },
        { 96, 3, "96 → C" },
        { 100, 3, "100 → C" },
        { 127, 3, "127 → C" },
        { 128, 4, "128 → B" },
        { 159, 4, "159 → B" },
        { 160, 5, "160 → A" },
        { 191, 5, "191 → A" },
        { 192, 6, "192 → S" },
        { 223, 6, "223 → S" },
        { 224, 7, "224 → SS" },
        { 255, 7, "255 → SS" },
        { 256, 7, "256 → SS clamp" },
        { 999, 7, "999 → SS clamp" },
        // mid-band interiors
        { 15, 0, "mid F" },
        { 47, 1, "mid E" },
        { 80, 2, "mid D" },
        { 111, 3, "mid C" },
        { 140, 4, "mid B" },
        { 175, 5, "mid A" },
        { 200, 6, "mid S" },
        { 240, 7, "mid SS" },
    };

    for (const auto& c : cases)
    {
        const uint8 got       = NumberToRank(c.skill);
        const uint8 inlineGot = inlineNumberToRank(c.skill);
        ok                    = expect(got == c.want, c.label) && ok;
        ok                    = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // AffectionToAffectionRank dual-wire compose: NumberToRank(affection).
    const int32 affectionSamples[] = { -1, 0, 31, 32, 100, 255, 999 };
    for (const int32 aff : affectionSamples)
    {
        ok = expect(AffectionToAffectionRank(aff) == NumberToRank(aff), "affection compose") && ok;
        ok = expect(AffectionToAffectionRank(aff) == inlineNumberToRank(aff), "affection dual-wire") && ok;
    }

    // Production consumer pins (rank multipliers 0..7).
    ok = expect(NumberToRank(0) == 0, "F multiplier 0") && ok;
    ok = expect(NumberToRank(255) == 7, "SS multiplier 7") && ok;
    ok = expect(NumberToRank(100) == 3, "C multiplier 3") && ok;

    return ok;
}
