#include "test_zone_query_by_name_2647.h"

#include <iostream>
#include <string>
#include <vector>

#include "map/zone_query_by_name_capacity.h"

namespace
{

struct Entity
{
    std::string name;
};

} // namespace

auto runZoneQueryByName2647SelfTests() -> bool
{
    const auto cacheHit = zonequeryhelpers::PlanQueryByName("Goblin", true);
    const auto cacheMiss = zonequeryhelpers::PlanQueryByName("Goblin", false);
    const auto dynamic = zonequeryhelpers::PlanQueryByName("DE_Goblin", true);

    const std::vector<Entity> npcs{ { "Goblin Guide" }, { "Moogle" } };
    const std::vector<Entity> mobs{ { "Goblin Thug" }, { "Goblin Smith" } };
    std::vector<std::string> names;
    zonequeryhelpers::ResolveMatches(
        [&](const auto& callback)
        {
            for (const auto& npc : npcs)
            {
                callback(&npc);
            }
        },
        [&](const auto& callback)
        {
            for (const auto& mob : mobs)
            {
                callback(&mob);
            }
        },
        [](const Entity* entity) { return entity->name.starts_with("Goblin"); },
        [&](const Entity* entity) { names.emplace_back(entity->name); });

    bool ok = cacheHit.useCache && cacheHit.returnCached && cacheHit.cacheResult &&
              cacheMiss.useCache && !cacheMiss.returnCached && cacheMiss.cacheResult &&
              !dynamic.useCache && !dynamic.returnCached && dynamic.cacheResult &&
              names == std::vector<std::string>{ "Goblin Guide", "Goblin Thug", "Goblin Smith" };

    std::vector<std::string> empty;
    zonequeryhelpers::ResolveMatches(
        [&](const auto&) {}, [&](const auto&) {},
        [](const auto*) { return true; }, [&](const auto* entity) { empty.emplace_back(entity->name); });
    ok = empty.empty() && ok; // Caller stores this empty result because cacheResult is true.

    if (!ok)
    {
        std::cerr << "zone query-by-name self-test failed\n";
    }
    return ok;
}
