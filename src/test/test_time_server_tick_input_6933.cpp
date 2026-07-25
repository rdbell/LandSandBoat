#include "test_time_server_tick_input_6933.h"

#include "map/time_server_tick_input.h"

#include <chrono>
#include <iostream>

auto runTimeServerTickInput6933SelfTests() -> bool
{
    using namespace std::chrono_literals;

    const auto epoch = earth_time::time_point{ 1009810800s };
    const auto input = timeservertickinputhelpers::MakeInput(epoch);
    if (input.earthTime != epoch || input.jstHour != 0 || input.jstWeekday != 2 ||
        input.vanaHour != 0 || input.vanaTotd != vanadiel_time::TOTD::MIDNIGHT)
    {
        std::cerr << "time server tick input 6933 epoch self-test failed\n";
        return false;
    }

    const auto nextHour = timeservertickinputhelpers::MakeInput(epoch + 144s);
    if (nextHour.jstHour != 0 || nextHour.jstWeekday != 2 || nextHour.vanaHour != 1 ||
        nextHour.vanaTotd != vanadiel_time::TOTD::MIDNIGHT)
    {
        std::cerr << "time server tick input 6933 Vana-hour self-test failed\n";
        return false;
    }
    return true;
}
