#include "test_zone_for_each_char_8060.h"

#include "common/scheduler.h"
#include "map/entities/char_entity.h"
#include "map/map_config.h"
#include "map/zone.h"

#define private public
#include "map/zone_entities.h"
#undef private

#include <functional>
#include <iostream>
#include <set>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone for-each-char 8060 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runZoneForEachChar8060SelfTests() -> bool
{
    Scheduler     scheduler(1);
    CZoneEntities entities(scheduler, MapConfig{}, nullptr);

    bool emptyCalled = false;
    entities.ForEachChar(
        [&emptyCalled](CCharEntity*)
        {
            emptyCalled = true;
        });

    bool ok = expect(!emptyCalled, "empty list invokes no callback");

    bool emptyFunctionThrew = false;
    try
    {
        entities.ForEachChar({});
    }
    catch (const std::bad_function_call&)
    {
        emptyFunctionThrew = true;
    }
    ok = expect(!emptyFunctionThrew, "empty list does not invoke an empty callback") && ok;

    CCharEntity first;
    CCharEntity second;
    entities.m_charList.emplace(10, &first);
    entities.m_charList.emplace(20, nullptr);
    entities.m_charList.emplace(30, &second);

    std::vector<CCharEntity*> visited;
    entities.ForEachChar(
        [&visited](CCharEntity* character)
        {
            visited.emplace_back(character);
        });

    const std::multiset<CCharEntity*> want{ &first, nullptr, &second };
    const std::multiset<CCharEntity*> got{ visited.begin(), visited.end() };
    ok = expect(got == want, "callback receives every stored value once without filtering null") && ok;

    bool populatedFunctionThrew = false;
    try
    {
        entities.ForEachChar({});
    }
    catch (const std::bad_function_call&)
    {
        populatedFunctionThrew = true;
    }
    ok = expect(populatedFunctionThrew, "populated list invokes and propagates an empty callback failure") && ok;

    return ok;
}
