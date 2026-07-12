#include "test_char_trait_sync_1471.h"

#include "map/char_trait_sync_capacity.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

auto runCharTraitSync1471SelfTests() -> bool
{
    std::vector<std::string> calls{};
    std::uint16_t            traitID = 0;
    chartraitsynchelpers::Apply(
        0x1234,
        [&]() { calls.emplace_back("base"); },
        [&](const std::uint16_t id)
        {
            calls.emplace_back("character");
            traitID = id;
        });

    const bool ok = calls == std::vector<std::string>{ "base", "character" } && traitID == 0x1234;
    if (!ok)
    {
        std::cerr << "char trait sync 1471 self-test failed\n";
    }
    return ok;
}
