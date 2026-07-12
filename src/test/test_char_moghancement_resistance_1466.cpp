#include "test_char_moghancement_resistance_1466.h"

#include "map/char_moghancement_resistance_capacity.h"

#include <array>
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
    const auto handled = charmoghancementresistancehelpers::Apply(
        id,
        adding,
        [&](const Mod mod, const std::int16_t amount)
        {
            deltas.emplace_back(static_cast<std::uint16_t>(mod), amount);
        });
    return { handled, deltas };
}
} // namespace

auto runCharMoghancementResistance1466SelfTests() -> bool
{
    const std::array<std::pair<std::uint16_t, std::uint16_t>, 8> cases{
        std::pair{ 566, 255 },
        std::pair{ 2848, 240 },
        std::pair{ 2849, 241 },
        std::pair{ 2850, 242 },
        std::pair{ 2852, 244 },
        std::pair{ 2853, 246 },
        std::pair{ 2854, 245 },
        std::pair{ 2855, 248 },
    };

    bool ok = true;
    for (const auto& [id, mod] : cases)
    {
        ok = Apply(id, true) == std::pair{ true, std::vector<Delta>{ { mod, 10 } } } && ok;
        ok = Apply(id, false) == std::pair{ true, std::vector<Delta>{ { mod, -10 } } } && ok;
    }
    for (const auto id : { 0, 565, 567, 2851, 2856 })
    {
        ok = !Apply(static_cast<std::uint16_t>(id), true).first && Apply(static_cast<std::uint16_t>(id), true).second.empty() && ok;
    }

    if (!ok)
    {
        std::cerr << "char moghancement resistance 1466 self-test failed\n";
    }
    return ok;
}
