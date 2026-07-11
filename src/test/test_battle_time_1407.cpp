#include "test_battle_time_1407.h"

#include "map/battle_time_capacity.h"

#include <chrono>
#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle time 1407 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleTime1407SelfTests() -> bool
{
    using namespace std::chrono_literals;
    const auto epoch = std::chrono::steady_clock::time_point{};

    bool ok = expect(battletimehelpers::Elapsed(epoch + 125s, epoch + 5s) == 120s, "elapsed subtraction");
    ok      = expect(battletimehelpers::Elapsed(epoch + 5s, epoch + 5s) == 0s, "equal timestamps") && ok;
    ok      = expect(battletimehelpers::Elapsed(epoch + 4s, epoch + 5s) == -1s, "future start is not clamped") && ok;
    return ok;
}
