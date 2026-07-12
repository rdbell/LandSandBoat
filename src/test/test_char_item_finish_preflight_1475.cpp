#include "test_char_item_finish_preflight_1475.h"

#include "map/char_item_finish_preflight_capacity.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace
{
auto Run(bool equipment, std::uint32_t quantity, std::uint32_t reserve, std::uint16_t validTarget, std::size_t targets, std::uint8_t& flags) -> std::pair<bool, std::vector<std::string>>
{
    std::vector<std::string> calls{};
    const auto result = charitemfinishpreflighthelpers::Apply(
        equipment, quantity, reserve, validTarget, 7, 8,
        [&]() { calls.emplace_back("warn"); },
        [&]() { calls.emplace_back("failure"); },
        [&]() { calls.emplace_back("reset"); },
        [&](const std::uint8_t findFlags, const std::uint16_t)
        {
            calls.emplace_back("find");
            flags = findFlags;
            return targets;
        },
        [&](const std::uint32_t actorID, const std::uint16_t itemID)
        {
            calls.emplace_back(actorID == 7 && itemID == 8 ? "initialize" : "bad-ids");
        });
    return { result, calls };
}
} // namespace

auto runCharItemFinishPreflight1475SelfTests() -> bool
{
    std::uint8_t flags = 255;
    const auto quantity = Run(false, 0, 0, 0, 1, flags);
    bool ok = !quantity.first && quantity.second == std::vector<std::string>{ "warn", "failure" };
    const auto reserve = Run(false, 1, 1, 0, 1, flags);
    ok = !reserve.first && reserve.second == std::vector<std::string>{ "warn", "failure" } && ok;
    const auto equipment = Run(true, 0, 1, 0, 1, flags);
    ok = equipment.first && equipment.second == std::vector<std::string>{ "reset", "find", "initialize" } && flags == 0 && ok;
    const auto dead = Run(false, 1, 0, 0x0020, 1, flags);
    ok = dead.first && flags == 1 && ok;
    const auto untargetable = Run(false, 1, 0, 0, 0, flags);
    ok = !untargetable.first && untargetable.second == std::vector<std::string>{ "reset", "find" } && ok;
    if (!ok)
    {
        std::cerr << "char item finish preflight 1475 self-test failed\n";
    }
    return ok;
}
