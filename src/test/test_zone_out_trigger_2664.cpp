#include "test_zone_out_trigger_2664.h"
#include "map/zone_capacity.h"
#include <list>
#include <memory>

struct Area
{
    uint32 id;

    auto getTriggerAreaID() const -> uint32
    {
        return id;
    }
};

auto runZoneOutTrigger2664SelfTests() -> bool
{
    std::list<std::unique_ptr<Area>> a;
    a.emplace_back(std::make_unique<Area>(9));
    a.emplace_back(std::make_unique<Area>(3));
    a.emplace_back(std::make_unique<Area>(7));
    const auto it = zonehelpers::FirstZoneOutTriggerArea(a, [](uint32 id)
                                                         {
                                                             return id == 3 || id == 7;
                                                         });
    if (it == a.end() || (*it)->id != 3)
    {
        return false;
    }

    const auto missing = zonehelpers::FirstZoneOutTriggerArea(a, [](uint32 id)
                                                              {
                                                                  return id == 4;
                                                              });
    return missing == a.end();
}
