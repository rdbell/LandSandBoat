#include "test_char_death_apply_1480.h"

#include "map/char_death_apply_capacity.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
auto Run(const chardeathplanhelpers::Plan& plan, float& retain) -> std::vector<std::string>
{
    std::vector<std::string> calls{};
    chardeathapplyhelpers::Apply(
        plan,
        [&](chardeathplanhelpers::Message) { calls.emplace_back("message"); },
        [&]() { calls.emplace_back("claim"); },
        [&]() { calls.emplace_back("pet"); },
        [&]() { calls.emplace_back("timed-death"); },
        [&]() { calls.emplace_back("death-time"); },
        [&]() { calls.emplace_back("block-aid"); },
        [&]() { calls.emplace_back("influence"); },
        [&](const float value) { calls.emplace_back("experience"); retain = value; },
        [&]() { calls.emplace_back("lua"); });
    return calls;
}
} // namespace

auto runCharDeathApply1480SelfTests() -> bool
{
    float retain = 0.0F;
    const auto full = Run({ .message = chardeathplanhelpers::Message::DefeatedBy, .despawnPet = true, .loseEXP = true, .retainPercent = 0.25F }, retain);
    bool ok = full == std::vector<std::string>{
        "message", "claim", "pet", "timed-death", "death-time", "block-aid", "influence", "experience", "lua",
    } && retain == 0.25F;
    const auto minimal = Run({}, retain);
    ok = minimal == std::vector<std::string>{
        "message", "claim", "timed-death", "death-time", "block-aid", "influence", "lua",
    } && ok;
    if (!ok)
    {
        std::cerr << "char death apply 1480 self-test failed\n";
    }
    return ok;
}
