#include "test_merit_is_merit_exist_3196.h"

#include "map/merit_capacity.h"

#include <iostream>

namespace
{

// Production category bounds (merit.h MERIT_CATEGORY).
constexpr int16 kCategoryStart = 0x0040; // MCATEGORY_START
constexpr int16 kCategoryCount = 0x0DC0; // MCATEGORY_COUNT

// Production GetMeritID: ((merit) & 0x3F) >> 1
auto meritIDOf(const int16 merit) -> uint8
{
    return static_cast<uint8>((static_cast<uint16>(merit) & 0x3F) >> 1);
}

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "merit IsMeritExist 3196 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline IsMeritExist compound bounds formula for dual-wire cross-check
// (slice 3196; residual 2816):
//   merit >= categoryStart && merit < categoryCount && meritID < meritsInCat
auto inlineIsMeritExist(
    const int16 merit,
    const int16 categoryStart,
    const int16 categoryCount,
    const uint8 meritID,
    const uint8 meritsInCat) -> bool
{
    return merit >= categoryStart && merit < categoryCount && meritID < meritsInCat;
}

// Positive if/else pin matching free function / capacity body (slice 3196).
// Avoid De Morgan rewrites of the compound-not form.
auto pinIsMeritExist(
    const int16 merit,
    const int16 categoryStart,
    const int16 categoryCount,
    const uint8 meritID,
    const uint8 meritsInCat) -> bool
{
    if (merit < categoryStart)
    {
        return false;
    }
    if (merit >= categoryCount)
    {
        return false;
    }
    if (meritID >= meritsInCat)
    {
        return false;
    }
    return true;
}

} // namespace

// Pure dual-wire expansion for meritshelpers::IsMeritExist
// (range + MeritsInCat bounds gate; slice 3196). Residual 2816 pins still hold;
// free == inline == pin (positive if/else). Poles: below start, at/above count,
// meritID overflow, valid.
auto runMeritIsMeritExist3196SelfTests() -> bool
{
    using meritshelpers::IsMeritExist;

    bool ok = true;

    // Residual 2816 pins still hold under dual-wire.
    ok = expect(!IsMeritExist(0, kCategoryStart, kCategoryCount, 0, 3), "residual merit 0 below start") && ok;
    ok = expect(!IsMeritExist(kCategoryStart - 1, kCategoryStart, kCategoryCount, 0, 3), "residual just below start") && ok;
    ok = expect(!IsMeritExist(-1, kCategoryStart, kCategoryCount, 0, 3), "residual negative merit") && ok;
    ok = expect(!IsMeritExist(kCategoryCount, kCategoryStart, kCategoryCount, 0, 3), "residual merit == categoryCount") && ok;
    ok = expect(!IsMeritExist(kCategoryCount + 1, kCategoryStart, kCategoryCount, 0, 3), "residual merit past categoryCount") && ok;

    {
        constexpr int16 maxHP    = 0x0040;
        constexpr int16 maxMP    = 0x0042;
        constexpr int16 maxMerit = 0x0044;
        constexpr int16 id3      = 0x0046;
        constexpr uint8 catSize  = 3;

        ok = expect(IsMeritExist(maxHP, kCategoryStart, kCategoryCount, meritIDOf(maxHP), catSize), "residual MAX_HP exists") && ok;
        ok = expect(IsMeritExist(maxMP, kCategoryStart, kCategoryCount, meritIDOf(maxMP), catSize), "residual MAX_MP exists") && ok;
        ok = expect(IsMeritExist(maxMerit, kCategoryStart, kCategoryCount, meritIDOf(maxMerit), catSize), "residual MAX_MERIT exists") && ok;
        ok = expect(!IsMeritExist(id3, kCategoryStart, kCategoryCount, meritIDOf(id3), catSize), "residual HP/MP id 3 absent") && ok;
    }

    {
        constexpr int16 berserk = 0x0180;
        constexpr int16 doubleA = 0x0188;
        constexpr int16 id5     = 0x018A;
        constexpr uint8 catSize = 5;

        ok = expect(IsMeritExist(berserk, kCategoryStart, kCategoryCount, meritIDOf(berserk), catSize), "residual BERSERK exists") && ok;
        ok = expect(IsMeritExist(doubleA, kCategoryStart, kCategoryCount, meritIDOf(doubleA), catSize), "residual DOUBLE_ATTACK exists") && ok;
        ok = expect(!IsMeritExist(id5, kCategoryStart, kCategoryCount, meritIDOf(id5), catSize), "residual WAR1 id 5 absent") && ok;
    }

    {
        constexpr int16 unkBase = 0x0740;
        ok = expect(!IsMeritExist(unkBase, kCategoryStart, kCategoryCount, meritIDOf(unkBase), 0), "residual UNK MeritsInCat=0") && ok;
        ok = expect(!IsMeritExist(unkBase, kCategoryStart, kCategoryCount, 0, 0), "residual id0 vs MeritsInCat=0") && ok;
    }

    ok = expect(IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 2, 3), "residual id == size-1 present") && ok;
    ok = expect(!IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 3, 3), "residual id == size absent") && ok;
    ok = expect(!IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 4, 3), "residual id > size absent") && ok;

    {
        constexpr int16 start = 10;
        constexpr int16 count = 20;
        ok = expect(!IsMeritExist(9, start, count, 0, 5), "residual scaled below start") && ok;
        ok = expect(IsMeritExist(10, start, count, 0, 5), "residual scaled at start") && ok;
        ok = expect(IsMeritExist(19, start, count, 4, 5), "residual scaled last in range") && ok;
        ok = expect(!IsMeritExist(20, start, count, 0, 5), "residual scaled at count") && ok;
        ok = expect(!IsMeritExist(15, start, count, 5, 5), "residual scaled id at size") && ok;
    }

    ok = expect(IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 0, 1), "residual exact start id0 size1") && ok;
    ok = expect(!IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 0, 0), "residual exact start empty cat") && ok;
    ok = expect(IsMeritExist(kCategoryCount - 1, kCategoryStart, kCategoryCount, 0, 1), "residual just under count present") && ok;

    const struct
    {
        int16       merit;
        int16       categoryStart;
        int16       categoryCount;
        uint8       meritID;
        uint8       meritsInCat;
        bool        want;
        const char* label;
    } cases[] = {
        // Below start poles.
        { 0, kCategoryStart, kCategoryCount, 0, 3, false, "merit 0 below start" },
        { kCategoryStart - 1, kCategoryStart, kCategoryCount, 0, 3, false, "just below start" },
        { -1, kCategoryStart, kCategoryCount, 0, 3, false, "negative merit below start" },
        { 9, 10, 20, 0, 5, false, "scaled below start" },

        // At / above categoryCount poles.
        { kCategoryCount, kCategoryStart, kCategoryCount, 0, 3, false, "merit == categoryCount" },
        { kCategoryCount + 1, kCategoryStart, kCategoryCount, 0, 3, false, "merit past categoryCount" },
        { 20, 10, 20, 0, 5, false, "scaled at count" },
        { 21, 10, 20, 0, 5, false, "scaled past count" },

        // meritID overflow poles.
        { kCategoryStart, kCategoryStart, kCategoryCount, 3, 3, false, "id == MeritsInCat overflow" },
        { kCategoryStart, kCategoryStart, kCategoryCount, 4, 3, false, "id > MeritsInCat overflow" },
        { kCategoryStart, kCategoryStart, kCategoryCount, 0, 0, false, "empty cat MeritsInCat=0" },
        { 15, 10, 20, 5, 5, false, "scaled id at size overflow" },
        { 0x0046, kCategoryStart, kCategoryCount, meritIDOf(0x0046), 3, false, "HP/MP id 3 overflow" },

        // Valid poles.
        { kCategoryStart, kCategoryStart, kCategoryCount, 0, 3, true, "exact start id0 size3" },
        { kCategoryStart, kCategoryStart, kCategoryCount, 2, 3, true, "id == size-1 present" },
        { 0x0040, kCategoryStart, kCategoryCount, meritIDOf(0x0040), 3, true, "MAX_HP exists" },
        { 0x0044, kCategoryStart, kCategoryCount, meritIDOf(0x0044), 3, true, "MAX_MERIT exists" },
        { kCategoryCount - 1, kCategoryStart, kCategoryCount, 0, 1, true, "just under count present" },
        { 10, 10, 20, 0, 5, true, "scaled at start valid" },
        { 19, 10, 20, 4, 5, true, "scaled last in range valid" },
        { kCategoryStart, kCategoryStart, kCategoryCount, 0, 1, true, "exact start id0 size1" },
    };

    for (const auto& c : cases)
    {
        const bool got     = IsMeritExist(c.merit, c.categoryStart, c.categoryCount, c.meritID, c.meritsInCat);
        const bool inlineF = inlineIsMeritExist(c.merit, c.categoryStart, c.categoryCount, c.meritID, c.meritsInCat);
        const bool pinF    = pinIsMeritExist(c.merit, c.categoryStart, c.categoryCount, c.meritID, c.meritsInCat);
        const bool wantPin = c.merit >= c.categoryStart && c.merit < c.categoryCount && c.meritID < c.meritsInCat;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "IsMeritExist dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "IsMeritExist dual-wire == positive if/else pin") && ok;
        ok = expect(got == wantPin, "IsMeritExist == pin formula AND gate") && ok;
    }

    // Pin composition: admit exist iff all three gates pass.
    ok = expect(IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 0, 3), "exact under all gates must admit") && ok;
    ok = expect(!IsMeritExist(kCategoryStart - 1, kCategoryStart, kCategoryCount, 0, 3), "start gate alone must block") && ok;
    ok = expect(!IsMeritExist(kCategoryCount, kCategoryStart, kCategoryCount, 0, 3), "count gate alone must block") && ok;
    ok = expect(!IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 3, 3), "meritID gate alone must block") && ok;

    // Host-style inject dual-wire: free == inline == pin across residual poles.
    for (const auto& c : cases)
    {
        const bool got  = IsMeritExist(c.merit, c.categoryStart, c.categoryCount, c.meritID, c.meritsInCat);
        const bool want = c.merit >= c.categoryStart && c.merit < c.categoryCount && c.meritID < c.meritsInCat;
        ok              = expect(got == want, "host inject dual-wire pin") && ok;
        ok              = expect(got == inlineIsMeritExist(c.merit, c.categoryStart, c.categoryCount, c.meritID, c.meritsInCat),
                    "host inject free == inline") &&
             ok;
        ok = expect(got == pinIsMeritExist(c.merit, c.categoryStart, c.categoryCount, c.meritID, c.meritsInCat),
                    "host inject free == pin") &&
             ok;
    }

    // Explicit polarity: each gate can independently block.
    ok = expect(!IsMeritExist(kCategoryStart - 1, kCategoryStart, kCategoryCount, 0, 3), "polarity start block") && ok;
    ok = expect(!IsMeritExist(kCategoryCount, kCategoryStart, kCategoryCount, 0, 3), "polarity count block") && ok;
    ok = expect(!IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 3, 3), "polarity meritID block") && ok;
    ok = expect(IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 0, 3), "polarity all clear") && ok;
    ok = expect(IsMeritExist(10, 10, 20, 0, 1), "polarity minimal admit") && ok;

    // Sibling dual-wires left alone (independent surfaces).
    ok = expect(meritshelpers::ShouldRaiseMerit(1, 1, 0, 15, 0, 75), "sibling ShouldRaiseMerit (3160) still admits") && ok;
    ok = expect(meritshelpers::ShouldLowerMerit(1), "sibling ShouldLowerMerit (3054) still admits") && ok;

    return ok;
}
