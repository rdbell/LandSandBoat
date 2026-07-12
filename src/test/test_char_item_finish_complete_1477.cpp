#include "test_char_item_finish_complete_1477.h"

#include "map/char_item_finish_complete_capacity.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace
{
auto Run(bool equipment, std::uint8_t maxCharges, std::uint8_t currentCharges, std::uint8_t slot, std::uint8_t location, std::uint16_t& key) -> std::pair<bool, std::vector<std::string>>
{
    std::vector<std::string> calls{};
    const auto commit = charitemfinishcompletehelpers::Apply(
        equipment, maxCharges, currentCharges, slot, location,
        [&](const std::uint8_t charges) { calls.emplace_back("charges-" + std::to_string(charges)); },
        [&]() { calls.emplace_back("last-use"); },
        [&]() { calls.emplace_back("persist"); },
        [&](const std::uint16_t recastKey) { calls.emplace_back("recast"); key = recastKey; });
    return { commit, calls };
}
} // namespace

auto runCharItemFinishComplete1477SelfTests() -> bool
{
    std::uint16_t key = 0;
    const auto consumable = Run(false, 0, 0, 0, 0, key);
    bool ok = consumable.first && consumable.second.empty();
    const auto charged = Run(true, 2, 2, 3, 4, key);
    ok = !charged.first && charged.second == std::vector<std::string>{ "charges-1", "last-use", "persist", "recast" } && key == 0x0304 && ok;
    key = 0;
    const auto single = Run(true, 1, 1, 5, 6, key);
    ok = !single.first && single.second == std::vector<std::string>{ "last-use", "persist", "recast" } && key == 0x0506 && ok;
    key = 0;
    const auto last = Run(true, 2, 1, 7, 8, key);
    ok = !last.first && last.second == std::vector<std::string>{ "charges-0", "last-use", "persist" } && key == 0 && ok;
    if (!ok)
    {
        std::cerr << "char item finish complete 1477 self-test failed\n";
    }
    return ok;
}
