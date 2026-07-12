#include "test_char_timed_death_1481.h"

#include "map/char_timed_death_capacity.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace
{
auto CheckPlanning() -> bool
{
    const auto fresh = chartimeddeathhelpers::Build({ .weaknessLevel = 9 });
    const auto weak  = chartimeddeathhelpers::Build({ .hasWeakness = true, .weaknessLevel = 2 });
    const auto wrap  = chartimeddeathhelpers::Build({ .hasWeakness = true, .weaknessLevel = 255 });
    const auto tiers = chartimeddeathhelpers::Build({ .reraiseI = true, .reraiseII = true, .reraiseIII = true });
    const auto mijin = chartimeddeathhelpers::Build({ .mijinReraise = true });
    const auto keep  = chartimeddeathhelpers::Build({ .hasRaise = 2, .mijinReraise = true });
    return !fresh.removeWeakness && fresh.weaknessLevel == 0 && fresh.hasRaise == 0 &&
           weak.removeWeakness && weak.weaknessLevel == 3 && wrap.weaknessLevel == 0 &&
           tiers.hasRaise == 3 && mijin.hasRaise == 1 && keep.hasRaise == 2;
}

auto CheckApplication() -> bool
{
    std::vector<std::string> calls{};
    std::uint8_t             weakness = 0;
    std::uint8_t             raise    = 0;
    auto                     duration = std::chrono::seconds::zero();
    chartimeddeathhelpers::Apply(
        { .removeWeakness = true, .weaknessLevel = 3, .hasRaise = 2 },
        std::chrono::seconds(5),
        [&]() { calls.emplace_back("trusts"); },
        [&]() { calls.emplace_back("remove-weakness"); },
        [&](const std::uint8_t level) { calls.emplace_back("set-weakness"); weakness = level; },
        [&]() { calls.emplace_back("death-sync"); },
        [&]() { calls.emplace_back("clear-ai"); },
        [&](const std::chrono::seconds value) { calls.emplace_back("internal-die"); duration = value; },
        [&]() { calls.emplace_back("allegiance"); },
        [&](const std::uint8_t level) { calls.emplace_back("set-raise"); raise = level; },
        [&]() { calls.emplace_back("knockout"); },
        [&]() { calls.emplace_back("base-die"); });
    return calls == std::vector<std::string>{
               "trusts", "remove-weakness", "set-weakness", "death-sync", "clear-ai",
               "internal-die", "allegiance", "set-raise", "knockout", "base-die",
           } &&
           weakness == 3 && raise == 2 && duration == std::chrono::seconds(5);
}
} // namespace

auto runCharTimedDeath1481SelfTests() -> bool
{
    const bool ok = CheckPlanning() && CheckApplication();
    if (!ok)
    {
        std::cerr << "char timed death 1481 self-test failed\n";
    }
    return ok;
}
