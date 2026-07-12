#include "test_char_raise_apply_1474.h"

#include "map/char_raise_apply_capacity.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace
{
auto Run(const charraiseplanhelpers::Plan& plan, std::uint16_t expLost, std::uint16_t& returnedXP) -> std::vector<std::string>
{
    std::vector<std::string> calls{};
    charraiseapplyhelpers::Apply(
        plan,
        [&](std::uint8_t) { calls.emplace_back("set-weakness"); },
        [&](std::uint8_t, std::chrono::minutes) { calls.emplace_back("add-weakness"); },
        [&](ActionAnimation) { calls.emplace_back("prepare-action"); },
        [&](std::uint16_t) { calls.emplace_back("add-hp"); },
        [&]() { calls.emplace_back("mark-hp"); },
        [&]() { calls.emplace_back("push-action"); },
        [&]() { calls.emplace_back("get-exp"); return expLost; },
        [&](std::uint16_t xp) { calls.emplace_back("add-exp"); returnedXP = xp; },
        [&]() { calls.emplace_back("clear-exp"); },
        [&]() { calls.emplace_back("add-reraise"); },
        [&]() { calls.emplace_back("clear-mijin"); },
        [&]() { calls.emplace_back("clear-arise"); },
        [&]() { calls.emplace_back("clear-raise"); });
    return calls;
}
} // namespace

auto runCharRaiseApply1474SelfTests() -> bool
{
    std::uint16_t returnedXP = 0;
    const auto full = Run({
        .handled = true, .weaknessLevel = 2, .applyWeakness = true, .weaknessDuration = std::chrono::minutes{ 3 },
        .animation = ActionAnimation::Arise, .hpReturned = 1000, .expReturnRatio = 0.5, .applyReraise3 = true,
    }, 101, returnedXP);
    bool ok = full == std::vector<std::string>{
        "set-weakness", "add-weakness", "prepare-action", "add-hp", "mark-hp", "push-action", "get-exp",
        "add-exp", "clear-exp", "add-reraise", "clear-mijin", "clear-arise", "clear-raise",
    } && returnedXP == 51;

    returnedXP = 0;
    const auto minimal = Run({ .handled = true, .weaknessLevel = 1, .hpReturned = 1 }, 0, returnedXP);
    ok = minimal == std::vector<std::string>{
        "set-weakness", "prepare-action", "add-hp", "mark-hp", "push-action", "get-exp",
        "clear-mijin", "clear-arise", "clear-raise",
    } && returnedXP == 0 && ok;

    const auto none = Run({}, 101, returnedXP);
    ok = none.empty() && ok;
    if (!ok)
    {
        std::cerr << "char raise apply 1474 self-test failed\n";
    }
    return ok;
}
