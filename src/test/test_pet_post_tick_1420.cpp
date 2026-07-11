#include "test_pet_post_tick_1420.h"

#include "map/pet_post_tick_capacity.h"

#include <iostream>
#include <vector>

auto runPetPostTick1420SelfTests() -> bool
{
    using clock = std::chrono::steady_clock;
    const auto epoch = clock::time_point{};
    bool ok = petposttickhelpers::ShouldSendUpdate(true, true, false, true) &&
              !petposttickhelpers::ShouldSendUpdate(false, true, false, true) &&
              !petposttickhelpers::ShouldSendUpdate(true, false, false, true) &&
              !petposttickhelpers::ShouldSendUpdate(true, true, true, true) &&
              !petposttickhelpers::ShouldSendUpdate(true, true, false, false);
    ok = ok && petposttickhelpers::ShouldSyncMaster(true, true) &&
         !petposttickhelpers::ShouldSyncMaster(false, true) &&
         !petposttickhelpers::ShouldSyncMaster(true, false);

    std::vector<int> calls;
    auto scheduled = epoch;
    petposttickhelpers::Apply(
        [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); return epoch + std::chrono::seconds(1); },
        [&]() { calls.push_back(3); return true; }, [&]() { calls.push_back(4); return true; },
        [&]() { calls.push_back(5); return false; }, [&]() { calls.push_back(6); return epoch; },
        [&](auto next) { calls.push_back(7); scheduled = next; }, [&]() { calls.push_back(8); },
        [&]() { calls.push_back(9); return true; }, [&]() { calls.push_back(10); }, [&]() { calls.push_back(11); });
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } &&
         scheduled == epoch + std::chrono::seconds(1) + petposttickhelpers::UpdateInterval;
    calls.clear();
    petposttickhelpers::Apply(
        [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); return epoch; },
        [&]() { calls.push_back(3); return true; }, [&]() { calls.push_back(4); return true; },
        [&]() { calls.push_back(5); return false; }, [&]() { calls.push_back(6); return epoch; },
        [&](auto) { calls.push_back(7); }, [&]() { calls.push_back(8); },
        [&]() { calls.push_back(9); return true; }, [&]() { calls.push_back(10); }, [&]() { calls.push_back(11); });
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 5, 6 };
    if (!ok)
    {
        std::cerr << "pet post tick 1420 self-test failed\n";
    }
    return ok;
}
