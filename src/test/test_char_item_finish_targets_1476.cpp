#include "test_char_item_finish_targets_1476.h"

#include "map/char_item_finish_targets_capacity.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

auto runCharItemFinishTargets1476SelfTests() -> bool
{
    std::vector<std::string> calls{};
    std::vector<int> area{ 8, 9 };
    charitemfinishtargetshelpers::Apply(
        false, 7, 1, 0x20,
        [&]() { calls.emplace_back("reset"); },
        [&](float, std::uint8_t, std::uint16_t) -> const auto& { calls.emplace_back("find"); return area; },
        [&](int target) { calls.emplace_back("process-" + std::to_string(target)); });
    bool ok = calls == std::vector<std::string>{ "process-7" };

    calls.clear();
    charitemfinishtargetshelpers::Apply(
        true, 7, 1, 0x20,
        [&]() { calls.emplace_back("reset"); },
        [&](const float radius, const std::uint8_t flags, const std::uint16_t validTarget) -> const auto&
        {
            calls.emplace_back(radius == 10.0F && flags == 1 && validTarget == 0x20 ? "find" : "bad-find");
            return area;
        },
        [&](int target) { calls.emplace_back("process-" + std::to_string(target)); });
    ok = calls == std::vector<std::string>{ "reset", "find", "process-8", "process-9" } && ok;

    area.clear();
    calls.clear();
    charitemfinishtargetshelpers::Apply(
        true, 7, 0, 0,
        [&]() { calls.emplace_back("reset"); },
        [&](float, std::uint8_t, std::uint16_t) -> const auto& { calls.emplace_back("find"); return area; },
        [&](int target) { calls.emplace_back("process-" + std::to_string(target)); });
    ok = calls == std::vector<std::string>{ "reset", "find" } && ok;

    if (!ok)
    {
        std::cerr << "char item finish targets 1476 self-test failed\n";
    }
    return ok;
}
