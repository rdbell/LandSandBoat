#include "test_mob_spell_storm_day_8881.h"

#include "map/mob_spell_container_capacity.h"
#include "omega_self_test_registry.h"

#include <array>
#include <iostream>

auto runMobSpellStormDay8881SelfTests() -> bool
{
    struct TestCase
    {
        std::size_t dayElement;
        SPELLFAMILY family;
    };

    constexpr std::array<TestCase, 8> tests = {
        TestCase{ 1, SPELLFAMILY_FIRESTORM },
        TestCase{ 2, SPELLFAMILY_HAILSTORM },
        TestCase{ 3, SPELLFAMILY_WINDSTORM },
        TestCase{ 4, SPELLFAMILY_SANDSTORM },
        TestCase{ 5, SPELLFAMILY_THUNDERSTORM },
        TestCase{ 6, SPELLFAMILY_RAINSTORM },
        TestCase{ 7, SPELLFAMILY_AURORASTORM },
        TestCase{ 8, SPELLFAMILY_VOIDSTORM },
    };

    bool ok = true;
    for (const auto& test : tests)
    {
        auto calls = 0;
        auto got   = mobspellhelpers::ResolveStormDaySpell(test.dayElement, [&](const SPELLFAMILY family) -> Maybe<SpellID> {
            ++calls;
            if (family != test.family)
            {
                ok = false;
            }
            return SpellID::Firestorm;
        });
        if (got != SpellID::Firestorm || calls != 1)
        {
            ok = false;
        }
    }

    for (const auto invalid : { std::size_t{ 0 }, std::size_t{ 9 } })
    {
        auto calls = 0;
        auto got   = mobspellhelpers::ResolveStormDaySpell(invalid, [&](const SPELLFAMILY) -> Maybe<SpellID> {
            ++calls;
            return SpellID::Firestorm;
        });
        if (got.has_value() || calls != 0)
        {
            ok = false;
        }
    }

    const auto unavailable = mobspellhelpers::ResolveStormDaySpell(
        std::size_t{ 1 }, [](const SPELLFAMILY) -> Maybe<SpellID> { return std::nullopt; });
    if (unavailable.has_value())
    {
        ok = false;
    }

    if (!ok)
    {
        std::cerr << "mob spell Storm day 8881 self-test failed\n";
    }
    return ok;
}

OMEGA_REGISTER_SELF_TEST("mob-spell-storm-day-8881", runMobSpellStormDay8881SelfTests);
