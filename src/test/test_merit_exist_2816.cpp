#include "test_merit_exist_2816.h"

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
        std::cerr << "merit exist 2816 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMeritExist2816SelfTests() -> bool
{
    using namespace meritshelpers;

    bool ok = true;

    // --- below category start ---
    ok = expect(!IsMeritExist(0, kCategoryStart, kCategoryCount, 0, 3), "merit 0 below start") && ok;
    ok = expect(!IsMeritExist(kCategoryStart - 1, kCategoryStart, kCategoryCount, 0, 3), "just below start") && ok;
    ok = expect(!IsMeritExist(-1, kCategoryStart, kCategoryCount, 0, 3), "negative merit") && ok;

    // --- at / past category count ---
    ok = expect(!IsMeritExist(kCategoryCount, kCategoryStart, kCategoryCount, 0, 3), "merit == categoryCount") && ok;
    ok = expect(!IsMeritExist(kCategoryCount + 1, kCategoryStart, kCategoryCount, 0, 3), "merit past categoryCount") && ok;

    // --- HP/MP category (MeritsInCat=3): ids 0..2 valid ---
    // MERIT_MAX_HP   = 0x0040 + 0x00 → id 0
    // MERIT_MAX_MP   = 0x0040 + 0x02 → id 1
    // MERIT_MAX_MERIT= 0x0040 + 0x04 → id 2
    // id 3 = 0x0040 + 0x06
    {
        constexpr int16 maxHP    = 0x0040;
        constexpr int16 maxMP    = 0x0042;
        constexpr int16 maxMerit = 0x0044;
        constexpr int16 id3      = 0x0046;
        constexpr uint8 catSize  = 3;

        ok = expect(IsMeritExist(maxHP, kCategoryStart, kCategoryCount, meritIDOf(maxHP), catSize), "MAX_HP exists") && ok;
        ok = expect(IsMeritExist(maxMP, kCategoryStart, kCategoryCount, meritIDOf(maxMP), catSize), "MAX_MP exists") && ok;
        ok = expect(IsMeritExist(maxMerit, kCategoryStart, kCategoryCount, meritIDOf(maxMerit), catSize), "MAX_MERIT exists") && ok;
        ok = expect(!IsMeritExist(id3, kCategoryStart, kCategoryCount, meritIDOf(id3), catSize), "HP/MP id 3 absent") && ok;
        ok = expect(meritIDOf(id3) == 3, "HP/MP id 3 math") && ok;
    }

    // --- WAR_1 category (MeritsInCat=5): ids 0..4 valid ---
    // MERIT_BERSERK_RECAST = 0x0180 + 0x00
    // MERIT_DOUBLE_ATTACK  = 0x0180 + 0x08 → id 4
    // id 5 = 0x0180 + 0x0A
    {
        constexpr int16 berserk = 0x0180;
        constexpr int16 doubleA = 0x0188;
        constexpr int16 id5     = 0x018A;
        constexpr uint8 catSize = 5;

        ok = expect(IsMeritExist(berserk, kCategoryStart, kCategoryCount, meritIDOf(berserk), catSize), "BERSERK exists") && ok;
        ok = expect(IsMeritExist(doubleA, kCategoryStart, kCategoryCount, meritIDOf(doubleA), catSize), "DOUBLE_ATTACK exists") && ok;
        ok = expect(!IsMeritExist(id5, kCategoryStart, kCategoryCount, meritIDOf(id5), catSize), "WAR1 id 5 absent") && ok;
        ok = expect(meritIDOf(id5) == 5, "WAR1 id 5 math") && ok;
    }

    // --- empty category (MeritsInCat=0, UNK slots): always absent ---
    {
        constexpr int16 unkBase = 0x0740; // MCATEGORY_UNK_1 style base in range
        ok = expect(!IsMeritExist(unkBase, kCategoryStart, kCategoryCount, meritIDOf(unkBase), 0), "UNK MeritsInCat=0") && ok;
        ok = expect(!IsMeritExist(unkBase, kCategoryStart, kCategoryCount, 0, 0), "id0 vs MeritsInCat=0") && ok;
    }

    // --- meritID == meritsInCat boundary (last valid is size-1) ---
    ok = expect(IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 2, 3), "id == size-1 present") && ok;
    ok = expect(!IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 3, 3), "id == size absent") && ok;
    ok = expect(!IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 4, 3), "id > size absent") && ok;

    // --- injected non-production bounds (scaled) ---
    {
        constexpr int16 start = 10;
        constexpr int16 count = 20;
        ok = expect(!IsMeritExist(9, start, count, 0, 5), "scaled below start") && ok;
        ok = expect(IsMeritExist(10, start, count, 0, 5), "scaled at start") && ok;
        ok = expect(IsMeritExist(19, start, count, 4, 5), "scaled last in range") && ok;
        ok = expect(!IsMeritExist(20, start, count, 0, 5), "scaled at count") && ok;
        ok = expect(!IsMeritExist(15, start, count, 5, 5), "scaled id at size") && ok;
    }

    // --- exact start edge present with valid id ---
    ok = expect(IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 0, 1), "exact start id0 size1") && ok;
    ok = expect(!IsMeritExist(kCategoryStart, kCategoryStart, kCategoryCount, 0, 0), "exact start empty cat") && ok;

    // --- just under categoryCount present ---
    ok = expect(IsMeritExist(kCategoryCount - 1, kCategoryStart, kCategoryCount, 0, 1), "just under count present") && ok;

    return ok;
}
