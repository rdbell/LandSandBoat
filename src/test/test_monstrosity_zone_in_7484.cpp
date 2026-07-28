#include "test_monstrosity_zone_in_7484.h"

#include "map/monstrosity.h"

#include <iostream>
#include <utility>

auto runMonstrosityZoneIn7484SelfTests() -> bool
{
    using monstrosity::PlanZoneIn;

    for (const auto [enabled, hasData] : { std::pair{ false, false }, std::pair{ false, true }, std::pair{ true, false } })
    {
        const auto blocked = PlanZoneIn(enabled, hasData, false, false);
        if (blocked.applyInstinctModifiers || blocked.addGestation || blocked.gestationDurationSeconds != 0 || blocked.sendFullUpdate || blocked.markLookUpdate)
        {
            std::cerr << "monstrosity zone-in: disabled or absent data failed\n";
            return false;
        }
    }

    const auto normal = PlanZoneIn(true, true, false, false);
    if (!normal.applyInstinctModifiers || !normal.addGestation || normal.gestationDurationSeconds != 18 * 60 * 60 || !normal.sendFullUpdate || !normal.markLookUpdate)
    {
        std::cerr << "monstrosity zone-in: normal zone failed\n";
        return false;
    }

    const auto belligerent = PlanZoneIn(true, true, false, true);
    if (!belligerent.addGestation || belligerent.gestationDurationSeconds != 60)
    {
        std::cerr << "monstrosity zone-in: belligerency duration failed\n";
        return false;
    }

    const auto feretory = PlanZoneIn(true, true, true, false);
    if (!feretory.applyInstinctModifiers || feretory.addGestation || feretory.gestationDurationSeconds != 0 || !feretory.sendFullUpdate || !feretory.markLookUpdate)
    {
        std::cerr << "monstrosity zone-in: Feretory failed\n";
        return false;
    }

    return true;
}
