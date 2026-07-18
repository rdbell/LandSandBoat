#include "test_attackutils_roll_rate_3906.h"

#include "map/utils/attackutils_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackutils RollRatePercent 3906 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineRollRatePercent(const int16 rateMod, const int roll0to99) -> bool
{
    return rateMod > 0 && roll0to99 < rateMod;
}
} // namespace

auto runAttackutilsRollRate3906SelfTests() -> bool
{
    using attackutilshelpers::RollRatePercent;
    bool ok = true;
    const struct { int16 rate; int roll; bool want; const char* label; } cases[] = {
        { -1, 0, false, "negative rate never procs" },
        { 0, 0, false, "zero rate never procs" },
        { 1, 0, true, "one percent accepts zero roll" },
        { 1, 1, false, "roll equal to rate is excluded" },
        { 25, 24, true, "last winning roll" },
        { 25, 25, false, "first losing roll" },
        { 100, 99, true, "100 percent accepts max valid roll" },
    };
    for (const auto& c : cases)
    {
        const auto got = RollRatePercent(c.rate, c.roll);
        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineRollRatePercent(c.rate, c.roll), "free helper equals host formula") && ok;
    }
    return ok;
}
