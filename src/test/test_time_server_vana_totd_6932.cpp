#include "test_time_server_vana_totd_6932.h"

#include "map/time_server_vana_totd.h"

#include <iostream>

auto runTimeServerVanaTotd6932SelfTests() -> bool
{
    auto tracker = timeservervanatotdhelpers::Tracker{ 4 };
    if (timeservervanatotdhelpers::HasChanged(tracker, 4) ||
        !timeservervanatotdhelpers::HasChanged(tracker, 5) ||
        tracker.previous != 4)
    {
        std::cerr << "time server Vana'diel TOD tracker 6932 initial self-test failed\n";
        return false;
    }

    timeservervanatotdhelpers::MarkHandled(tracker, 5);
    if (timeservervanatotdhelpers::HasChanged(tracker, 5) || tracker.previous != 5)
    {
        std::cerr << "time server Vana'diel TOD tracker 6932 advance self-test failed\n";
        return false;
    }
    return true;
}
