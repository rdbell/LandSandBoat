#include "test_map_player_kick_refresh_1294.h"

#include "map/player_kick_refresh.h"

#include <iostream>
#include <limits>

namespace
{

struct FakePlayer
{
    void ReloadPartyInc()
    {
        ++reloadCalls;
    }

    int reloadCalls{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map player kick refresh 1294 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testResolvedVictim() -> bool
{
    FakePlayer player{};
    uint32     lookedUpId{};
    int        lookupCalls{};

    mapipc::HandlePlayerKick(
        ipc::PlayerKick{ .victimId = std::numeric_limits<uint32>::max() },
        [&](const uint32 victimId)
        {
            ++lookupCalls;
            lookedUpId = victimId;
            return &player;
        });

    return expect(lookupCalls == 1, "resolved victim looked up once") &&
           expect(lookedUpId == std::numeric_limits<uint32>::max(), "full victim ID preserved") &&
           expect(player.reloadCalls == 1, "resolved victim party reload requested once");
}

auto testMissingVictim() -> bool
{
    uint32 lookedUpId{};
    int    lookupCalls{};
    mapipc::HandlePlayerKick(
        ipc::PlayerKick{},
        [&](const uint32 victimId) -> FakePlayer*
        {
            ++lookupCalls;
            lookedUpId = victimId;
            return nullptr;
        });

    return expect(lookupCalls == 1, "zero victim looked up once") && expect(lookedUpId == 0, "zero victim ID preserved");
}

} // namespace

auto runMapPlayerKickRefresh1294SelfTests() -> bool
{
    return testResolvedVictim() && testMissingVictim();
}
