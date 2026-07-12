#include "test_char_ability_paralyze_1487.h"

#include "map/char_ability_paralyze_capacity.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
using charabilityparalyzehelpers::Apply;
using charabilityparalyzehelpers::ShouldConsumeRecastOnParalyze;

auto Check() -> bool
{
    if (!ShouldConsumeRecastOnParalyze(1) ||
        ShouldConsumeRecastOnParalyze(charabilityparalyzehelpers::RecastSpecial) ||
        ShouldConsumeRecastOnParalyze(charabilityparalyzehelpers::RecastSpecial2))
    {
        return false;
    }

    std::vector<std::string> calls{};
    if (Apply(false, 1, [&]() { calls.emplace_back("recast"); }, [&]() { calls.emplace_back("interrupt"); }) ||
        !calls.empty())
    {
        return false;
    }

    calls.clear();
    if (!Apply(true, 1, [&]() { calls.emplace_back("recast"); }, [&]() { calls.emplace_back("interrupt"); }) ||
        calls != std::vector<std::string>{ "recast", "interrupt" })
    {
        return false;
    }

    calls.clear();
    if (!Apply(true, charabilityparalyzehelpers::RecastSpecial,
               [&]() { calls.emplace_back("recast"); },
               [&]() { calls.emplace_back("interrupt"); }) ||
        calls != std::vector<std::string>{ "interrupt" })
    {
        return false;
    }

    calls.clear();
    if (!Apply(true, charabilityparalyzehelpers::RecastSpecial2,
               [&]() { calls.emplace_back("recast"); },
               [&]() { calls.emplace_back("interrupt"); }) ||
        calls != std::vector<std::string>{ "interrupt" })
    {
        return false;
    }

    return true;
}
} // namespace

auto runCharAbilityParalyze1487SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char ability paralyze 1487 self-test failed\n";
    }
    return ok;
}
