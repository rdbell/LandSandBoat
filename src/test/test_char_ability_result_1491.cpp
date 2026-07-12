#include "test_char_ability_result_1491.h"

#include "map/char_ability_result_capacity.h"

#include <iostream>

namespace
{
using charabilityresulthelpers::ResolveAoETarget;
using charabilityresulthelpers::ResolveSingle;

auto CheckSingle() -> bool
{
    auto identity = [](const std::uint16_t m) { return m; };
    auto absorb   = [](const std::uint16_t m) { return static_cast<std::uint16_t>(m + 1000); };

    // Lua left message unchanged → ability default, None → UsesJobAbility.
    {
        const auto r = ResolveSingle(0, 0, charabilityresulthelpers::MsgNone, 5, identity);
        if (r.messageID != charabilityresulthelpers::MsgUsesJobAbility || r.param != 5)
        {
            return false;
        }
    }

    // Ability default message kept when non-None.
    {
        const auto r = ResolveSingle(0, 0, 42, 7, identity);
        if (r.messageID != 42 || r.param != 7)
        {
            return false;
        }
    }

    // Lua set a different message → keep Lua message.
    {
        const auto r = ResolveSingle(0, 55, 42, 3, identity);
        if (r.messageID != 55 || r.param != 3)
        {
            return false;
        }
    }

    // Negative value absorbs and negates param.
    {
        const auto r = ResolveSingle(0, 0, 42, -9, absorb);
        if (r.messageID != 1042 || r.param != 9)
        {
            return false;
        }
    }

    return true;
}

auto CheckAoE() -> bool
{
    auto aoe    = [](const std::uint16_t m) { return static_cast<std::uint16_t>(m + 10); };
    auto absorb = [](const std::uint16_t m) { return static_cast<std::uint16_t>(m + 1000); };

    {
        const auto r = ResolveAoETarget(true, 42, 5, aoe, absorb);
        if (r.messageID != 42 || r.param != 5)
        {
            return false;
        }
    }
    {
        const auto r = ResolveAoETarget(false, 42, 5, aoe, absorb);
        if (r.messageID != 52 || r.param != 5)
        {
            return false;
        }
    }
    {
        const auto r = ResolveAoETarget(false, 42, -4, aoe, absorb);
        if (r.messageID != 1052 || r.param != 4)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runCharAbilityResult1491SelfTests() -> bool
{
    const bool ok = CheckSingle() && CheckAoE();
    if (!ok)
    {
        std::cerr << "char ability result 1491 self-test failed\n";
    }
    return ok;
}
