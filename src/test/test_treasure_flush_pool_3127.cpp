#include "test_treasure_flush_pool_3127.h"

#include "map/treasure_pool_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure ShouldFlushPool 3127 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline flush entry formula for dual-wire cross-check (slice 3127):
//   itemCount != 0
auto inlineShouldFlushPool(const uint8 itemCount) -> bool
{
    return itemCount != 0;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldFlushPool
// (itemCount != 0 flush entry gate; slice 3127). Zero vs nonzero counts.
auto runTreasureFlushPool3127SelfTests() -> bool
{
    using treasurepoolhelpers::PlanFlush;
    using treasurepoolhelpers::ShouldFlushPool;
    using treasurepoolhelpers::ShouldRejectNullItem;
    using treasurepoolhelpers::ShouldRejectNullMember;
    using treasurepoolhelpers::ShouldSkipRareCheck;
    using treasurepoolhelpers::ShouldUpdatePoolForChar;

    bool ok = true;

    const struct
    {
        uint8       itemCount;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles: zero vs nonzero.
        { 0, false, "empty pool does not flush" },
        { 1, true, "one item flushes" },

        // Empty-adjacent / multi-slot extremes.
        { 2, true, "two items flush" },
        { 5, true, "mid count flushes" },
        { 10, true, "full pool flushes" },
        { 255, true, "uint8 max count flushes" },

        // Residual 1367 / 2781 pins.
        { 0, false, "residual empty no flush" },
        { 1, true, "residual one item flush" },
        { 10, true, "residual full pool flush" },
        { 255, true, "residual max count flush" },

        // Boundary neighbors around empty.
        { 0, false, "zero count boundary" },
        { 1, true, "one count boundary" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldFlushPool(c.itemCount);
        const bool inlineF = inlineShouldFlushPool(c.itemCount);
        const bool wantPin = c.itemCount != 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldFlushPool dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldFlushPool == pin formula itemCount != 0") && ok;
    }

    // Pin composition: any non-zero itemCount flushes; zero no-ops.
    ok = expect(!ShouldFlushPool(0), "itemCount == 0 must not flush") && ok;
    ok = expect(ShouldFlushPool(1), "itemCount == 1 must flush") && ok;
    ok = expect(ShouldFlushPool(10), "itemCount == 10 must flush") && ok;
    ok = expect(ShouldFlushPool(255), "itemCount == 255 must flush") && ok;

    // Dense compose over representative item counts (0..20).
    for (uint16 count = 0; count <= 20; ++count)
    {
        const uint8 c    = static_cast<uint8>(count);
        const bool  got  = ShouldFlushPool(c);
        const bool  want = c != 0;
        ok               = expect(got == want, "compose free == pin formula") && ok;
        ok               = expect(got == inlineShouldFlushPool(c), "compose free == inline") && ok;
    }

    // Explicit dual-wire identity: free == inline == (itemCount != 0).
    for (const uint8 c : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 5 }, uint8{ 10 }, uint8{ 80 }, uint8{ 255 } })
    {
        const bool freeF   = ShouldFlushPool(c);
        const bool inlineF = inlineShouldFlushPool(c);
        const bool pin     = c != 0;
        ok                 = expect(freeF == inlineF && freeF == pin, "dual-wire free==inline==pin") && ok;
    }

    // Residual 1367 pins still hold under dual-wire.
    ok = expect(ShouldFlushPool(1) && !ShouldFlushPool(0), "residual flush pins") && ok;

    // Sibling dual-wire independence: 3060 / 3067 / 3094 / 3112 remain distinct.
    ok = expect(ShouldRejectNullMember(true, false) && ShouldRejectNullMember(false, true),
                "sibling ShouldRejectNullMember dual-wire polarity") &&
         ok;
    ok = expect(!ShouldRejectNullMember(false, false), "sibling valid member proceeds") && ok;
    ok = expect(ShouldRejectNullItem(true) && !ShouldRejectNullItem(false),
                "sibling ShouldRejectNullItem dual-wire polarity") &&
         ok;
    ok = expect(ShouldSkipRareCheck(false, true) && !ShouldSkipRareCheck(true, true),
                "sibling ShouldSkipRareCheck dual-wire polarity") &&
         ok;
    ok = expect(ShouldUpdatePoolForChar(false) && !ShouldUpdatePoolForChar(true),
                "sibling ShouldUpdatePoolForChar dual-wire polarity") &&
         ok;

    // Host-style inject poles through PlanFlush.
    {
        const auto empty = PlanFlush(0);
        ok               = expect(!empty.runChecks, "flush empty → !RunChecks") && ok;
    }
    {
        const auto one = PlanFlush(1);
        ok             = expect(one.runChecks, "flush one → RunChecks") && ok;
    }
    {
        const auto full = PlanFlush(10);
        ok              = expect(full.runChecks, "flush full → RunChecks") && ok;
    }
    {
        const auto maxC = PlanFlush(255);
        ok              = expect(maxC.runChecks, "flush max → RunChecks") && ok;
    }

    // Dual-wire: free gate polarity matches flush plan disposition.
    for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 5 }, uint8{ 10 }, uint8{ 80 }, uint8{ 255 } })
    {
        const bool flush = ShouldFlushPool(count);
        ok               = expect(flush == inlineShouldFlushPool(count), "flush free == inline") && ok;
        ok               = expect(flush == (count != 0), "flush free == pin") && ok;

        const auto plan = PlanFlush(count);
        ok              = expect(plan.runChecks == flush, "RunChecks == free") && ok;
    }

    // Dense PlanFlush compose over 0..20: runChecks == free.
    for (uint16 count = 0; count <= 20; ++count)
    {
        const uint8 c = static_cast<uint8>(count);
        ok            = expect(PlanFlush(c).runChecks == ShouldFlushPool(c), "compose PlanFlush == free") && ok;
        ok            = expect(PlanFlush(c).runChecks == inlineShouldFlushPool(c), "compose PlanFlush == inline") && ok;
    }

    // Residual policy compose still wires free into host plan.
    ok = expect(!ShouldFlushPool(0), "compose free empty") && ok;
    {
        const auto plan = PlanFlush(0);
        ok              = expect(!plan.runChecks, "compose free empty inject") && ok;
    }
    ok = expect(ShouldFlushPool(1), "compose free one") && ok;
    {
        const auto plan = PlanFlush(1);
        ok              = expect(plan.runChecks, "compose free one inject") && ok;
    }
    ok = expect(ShouldFlushPool(10), "compose free full") && ok;
    {
        const auto plan = PlanFlush(10);
        ok              = expect(plan.runChecks, "compose free full inject") && ok;
    }

    return ok;
}
