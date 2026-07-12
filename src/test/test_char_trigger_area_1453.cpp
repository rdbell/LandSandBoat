#include "test_char_trigger_area_1453.h"

#include "map/char_trigger_area_capacity.h"

#include <cstdint>
#include <iostream>
#include <unordered_set>

auto runCharTriggerArea1453SelfTests() -> bool
{
    std::unordered_set<std::uint32_t> triggerAreas{};
    bool ok = !chartriggerareahelpers::Contains(triggerAreas, std::uint32_t{ 0 });

    chartriggerareahelpers::Enter(triggerAreas, std::uint32_t{ 0 });
    chartriggerareahelpers::Enter(triggerAreas, std::uint32_t{ 0 });
    chartriggerareahelpers::Enter(triggerAreas, std::uint32_t{ 0xFFFFFFFF });
    ok = triggerAreas.size() == 2 &&
         chartriggerareahelpers::Contains(triggerAreas, std::uint32_t{ 0 }) &&
         chartriggerareahelpers::Contains(triggerAreas, std::uint32_t{ 0xFFFFFFFF }) && ok;

    chartriggerareahelpers::Leave(triggerAreas, std::uint32_t{ 123 });
    ok = triggerAreas.size() == 2 && ok;

    chartriggerareahelpers::Leave(triggerAreas, std::uint32_t{ 0 });
    ok = !chartriggerareahelpers::Contains(triggerAreas, std::uint32_t{ 0 }) &&
         chartriggerareahelpers::Contains(triggerAreas, std::uint32_t{ 0xFFFFFFFF }) && ok;

    chartriggerareahelpers::Clear(triggerAreas);
    ok = triggerAreas.empty() &&
         !chartriggerareahelpers::Contains(triggerAreas, std::uint32_t{ 0xFFFFFFFF }) && ok;

    if (!ok)
    {
        std::cerr << "char trigger area 1453 self-test failed\n";
    }
    return ok;
}
