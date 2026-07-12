#include "test_char_moghancement_craft_1464.h"

#include "map/char_moghancement_craft_capacity.h"

#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace
{
using Delta = std::pair<std::uint16_t, std::int16_t>;

auto Apply(std::uint16_t id, bool adding) -> std::pair<bool, std::vector<Delta>>
{
    std::vector<Delta> deltas{};
    const auto handled = charmoghancementcrafthelpers::Apply(
        id,
        adding,
        [&](const Mod mod, const std::int16_t amount)
        {
            deltas.emplace_back(static_cast<std::uint16_t>(mod), amount);
        });
    return { handled, deltas };
}
} // namespace

auto runCharMoghancementCraft1464SelfTests() -> bool
{
    bool ok = !Apply(0, true).first && Apply(0, true).second.empty();

    for (std::uint16_t offset = 0; offset < 8; ++offset)
    {
        ok = Apply(512 + offset, true) == std::pair{ true, std::vector<Delta>{ { 917 + offset, 5 } } } && ok;
        ok = Apply(512 + offset, false) == std::pair{ true, std::vector<Delta>{ { 917 + offset, -5 } } } && ok;
    }
    for (std::uint16_t offset = 0; offset < 9; ++offset)
    {
        ok = Apply(523 + offset, true) == std::pair{ true, std::vector<Delta>{ { 127 + offset, 1 } } } && ok;
        ok = Apply(523 + offset, false) == std::pair{ true, std::vector<Delta>{ { 127 + offset, -1 } } } && ok;
        ok = Apply(553 + offset, true) == std::pair{ true, std::vector<Delta>{ { 127 + offset, 5 } } } && ok;
        ok = Apply(553 + offset, false) == std::pair{ true, std::vector<Delta>{ { 127 + offset, -5 } } } && ok;
    }

    ok = Apply(544, true) == std::pair{ true, std::vector<Delta>{ { 127, 1 } } } && ok;
    ok = Apply(544, false) == std::pair{ true, std::vector<Delta>{ { 127, -1 } } } && ok;
    for (std::uint16_t offset = 1; offset < 9; ++offset)
    {
        ok = Apply(544 + offset, true) ==
                 std::pair{ true, std::vector<Delta>{ { 127 + offset, 1 }, { 924 + offset, 5 } } } && ok;
        ok = Apply(544 + offset, false) ==
                 std::pair{ true, std::vector<Delta>{ { 127 + offset, -1 }, { 924 + offset, -5 } } } && ok;
    }

    if (!ok)
    {
        std::cerr << "char moghancement craft 1464 self-test failed\n";
    }
    return ok;
}
