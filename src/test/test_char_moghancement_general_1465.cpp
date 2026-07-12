#include "test_char_moghancement_general_1465.h"

#include "map/char_moghancement_general_capacity.h"

#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace
{
using Delta = std::pair<std::uint16_t, std::int16_t>;

auto Apply(std::uint16_t id, std::uint8_t nation, bool adding) -> std::pair<bool, std::vector<Delta>>
{
    std::vector<Delta> deltas{};
    const auto handled = charmoghancementgeneralhelpers::Apply(
        id,
        nation,
        adding,
        [&](const Mod mod, const std::int16_t amount)
        {
            deltas.emplace_back(static_cast<std::uint16_t>(mod), amount);
        });
    return { handled, deltas };
}

auto Negated(std::vector<Delta> deltas) -> std::vector<Delta>
{
    for (auto& [mod, amount] : deltas)
    {
        amount = static_cast<std::int16_t>(-amount);
    }
    return deltas;
}
} // namespace

auto runCharMoghancementGeneral1465SelfTests() -> bool
{
    const std::vector<std::pair<std::uint16_t, std::vector<Delta>>> cases{
        { 520, { { 914, 5 } } },
        { 521, { { 975, 36 } } },
        { 522, { { 916, 2 } } },
        { 532, { { 933, 6 } } },
        { 533, { { 934, 10 } } },
        { 534, {} },
        { 538, { { 1158, 10 } } },
        { 539, { { 935, 5 } } },
        { 540, { { 1158, 15 } } },
        { 541, { { 64, 25 }, { 65, 25 } } },
        { 542, { { 382, 10 }, { 915, 10 } } },
        { 562, { { 382, 15 } } },
        { 563, { { 915, 15 } } },
    };

    bool ok = !Apply(0, 0, true).first && Apply(0, 0, true).second.empty();
    for (const auto& [id, deltas] : cases)
    {
        ok = Apply(id, 0, true) == std::pair{ true, deltas } && ok;
        ok = Apply(id, 0, false) == std::pair{ true, Negated(deltas) } && ok;
    }

    for (std::uint8_t effectNation = 0; effectNation < 3; ++effectNation)
    {
        const auto id = static_cast<std::uint16_t>(535 + effectNation);
        for (std::uint8_t playerNation = 0; playerNation < 4; ++playerNation)
        {
            const auto deltas = playerNation == effectNation ? std::vector<Delta>{ { 933, 6 } } : std::vector<Delta>{};
            ok = Apply(id, playerNation, true) == std::pair{ true, deltas } && ok;
            ok = Apply(id, playerNation, false) == std::pair{ true, Negated(deltas) } && ok;
        }
    }

    if (!ok)
    {
        std::cerr << "char moghancement general 1465 self-test failed\n";
    }
    return ok;
}
