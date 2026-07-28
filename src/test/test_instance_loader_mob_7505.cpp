#include "test_instance_loader_mob_7505.h"

#include "map/instance_loader_mob.h"

#include <iostream>

auto runInstanceLoaderMob7505SelfTests() -> bool
{
    using namespace instanceloader;
    bool ok = true;
    const struct { float x, y, z; uint8 type; int16 charmable; MobLoadPlan want; const char* label; } cases[] = {
        { 0, 0, 0, 0, 5, {}, "zero position skips" },
        { 1, 0, 0, 0, 5, { true, false, 5 }, "ordinary preserves charmability" },
        { 0, 1, 0, MobTypeEvent, 5, { true, true, 0 }, "event always aggros and cannot charm" },
        { 0, 0, 1, MobTypeFished, 5, { true, false, 0 }, "fished cannot charm" },
        { 1, 1, 1, MobTypeBattlefield | MobTypeNotorious, -3, { true, false, 0 }, "battlefield notorious cannot charm" },
    };
    for (const auto& c : cases)
    {
        if (PlanMobLoad(c.x, c.y, c.z, c.type, c.charmable) != c.want)
        {
            std::cerr << "instance loader mob 7505 self-test failed: " << c.label << '\n';
            ok = false;
        }
    }
    return ok;
}
