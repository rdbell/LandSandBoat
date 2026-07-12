#include "test_char_ability_preflight_1484.h"

#include "map/char_ability_preflight_capacity.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
auto CheckGates() -> bool
{
    using charabilitypreflighthelpers::Evaluate;
    using charabilitypreflighthelpers::Result;

    std::vector<std::string> calls{};
    auto reset = [&]() { calls.emplace_back("reset"); };
    auto find  = [&](const std::uint8_t flags, const std::uint16_t valid) -> int {
        calls.emplace_back("find:" + std::to_string(flags) + ":" + std::to_string(valid));
        return 1;
    };
    auto wait = [&]() { calls.emplace_back("wait"); };
    auto amnesia = [&]() { calls.emplace_back("amnesia"); };

    // Recast gate short-circuits before amnesia/target-find.
    calls.clear();
    if (Evaluate(true, true, 0x0020, wait, amnesia, reset, find) != Result::WaitLonger ||
        calls != std::vector<std::string>{ "wait" })
    {
        return false;
    }

    // Amnesia gate short-circuits before target-find.
    calls.clear();
    if (Evaluate(false, true, 0x0020, wait, amnesia, reset, find) != Result::Amnesia ||
        calls != std::vector<std::string>{ "amnesia" })
    {
        return false;
    }

    // Empty target-find is a silent no-op failure with dead-flag resolution.
    calls.clear();
    auto emptyFind = [&](const std::uint8_t flags, const std::uint16_t valid) -> int {
        calls.emplace_back("find:" + std::to_string(flags) + ":" + std::to_string(valid));
        return 0;
    };
    if (Evaluate(false, false, 0x0020, wait, amnesia, reset, emptyFind) != Result::NoTarget ||
        calls != std::vector<std::string>{ "reset", "find:1:32" })
    {
        return false;
    }

    // Alive-only valid target uses findFlags 0 and proceeds when a target exists.
    calls.clear();
    if (Evaluate(false, false, 0x0001, wait, amnesia, reset, find) != Result::Proceed ||
        calls != std::vector<std::string>{ "reset", "find:0:1" })
    {
        return false;
    }

    return true;
}
} // namespace

auto runCharAbilityPreflight1484SelfTests() -> bool
{
    const bool ok = CheckGates();
    if (!ok)
    {
        std::cerr << "char ability preflight 1484 self-test failed\n";
    }
    return ok;
}
