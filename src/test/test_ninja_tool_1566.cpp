#include "test_ninja_tool_1566.h"

#include "map/ninja_tool_capacity.h"

#include <iostream>

namespace
{
using namespace ninjatoolhelpers;

auto Fail(const char* msg) -> bool
{
    std::cerr << "ninja_tool_1566: " << msg << "\n";
    return false;
}

auto Check() -> bool
{
    {
        const auto s = Substitute(Uchitake);
        if (!s || *s != Inoshishinofuda)
        {
            return Fail("Substitute Uchitake");
        }
    }
    {
        const auto s = Substitute(Shihei);
        if (!s || *s != Shikanofuda)
        {
            return Fail("Substitute Shihei");
        }
    }
    {
        const auto s = Substitute(Soshi);
        if (!s || *s != Chonofuda)
        {
            return Fail("Substitute Soshi");
        }
    }
    if (Substitute(Inoshishinofuda).has_value() || Substitute(0).has_value())
    {
        return Fail("Substitute unknown");
    }

    if (!IsElementalWheel(Uchitake) || IsElementalWheel(Inoshishinofuda) || IsElementalWheel(Shihei))
    {
        return Fail("IsElementalWheel");
    }

    {
        const auto r = ResolveWithSubstitute(Uchitake, true, false, false);
        if (!r.ok || r.toolID != Uchitake || r.usedSubstitute)
        {
            return Fail("Resolve preferred available");
        }
    }
    {
        const auto r = ResolveWithSubstitute(Uchitake, false, false, true);
        if (r.ok)
        {
            return Fail("Resolve non-NIN miss");
        }
    }
    {
        const auto r = ResolveWithSubstitute(Uchitake, false, true, true);
        if (!r.ok || r.toolID != Inoshishinofuda || !r.usedSubstitute)
        {
            return Fail("Resolve NIN substitute");
        }
    }
    {
        const auto r = ResolveWithSubstitute(9999, false, true, true);
        if (r.ok)
        {
            return Fail("Resolve unknown preferred");
        }
    }

    if (ExpertiseChance(20, 10) != 30)
    {
        return Fail("ExpertiseChance");
    }
    // roll > chance → consume
    if (ShouldConsume(50, 50))
    {
        return Fail("ShouldConsume equal boundary");
    }
    if (!ShouldConsume(50, 51))
    {
        return Fail("ShouldConsume above chance");
    }
    if (ShouldConsume(100, 99))
    {
        return Fail("ShouldConsume full expertise");
    }

    if (ConsumeQty(Uchitake, true, 0, 0) != 2)
    {
        return Fail("ConsumeQty Futae wheel");
    }
    if (ConsumeQty(Inoshishinofuda, true, 0, 99) != 1)
    {
        return Fail("ConsumeQty Futae non-wheel expertise");
    }
    if (ConsumeQty(Uchitake, false, 100, 0) != 0)
    {
        return Fail("ConsumeQty expertise save");
    }
    // chance 0: roll 0 does not consume (0 > 0 false); roll 1 does.
    if (ConsumeQty(Uchitake, false, 0, 0) != 0)
    {
        return Fail("ConsumeQty chance0 roll0 no consume");
    }
    if (ConsumeQty(Uchitake, false, 0, 1) != 1)
    {
        return Fail("ConsumeQty chance0 roll1 consume");
    }

    if (!NonPCAlwaysHasTool())
    {
        return Fail("NonPCAlwaysHasTool");
    }

    return true;
}
} // namespace

auto runNinjaTool1566SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "ninja_tool_1566 self-tests failed\n";
        return false;
    }
    return true;
}
