#include "test_map_char_zone_1254.h"

#include "map/char_zone.h"
#include "map/entities/char_entity.h"
#include "map/map_session.h"

#include <iostream>

namespace
{

struct FakeSession
{
    uint32 id{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map CharZone 1254 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testExistingSessionIsTouchedOnly() -> bool
{
    FakeSession session{ .id = 77 };
    uint32      lookedUpId{};
    int         lookupCalls{};
    int         touchCalls{};
    int         createCalls{};

    mapipc::HandleCharZone(
        ipc::CharZone{ .charId = session.id, .destinationZoneId = 0xFFFF },
        [&](const uint32 charId)
        {
            ++lookupCalls;
            lookedUpId = charId;
            return &session;
        },
        [&](FakeSession* touched)
        {
            if (touched == &session)
            {
                ++touchCalls;
            }
        },
        [&](const uint32)
        {
            ++createCalls;
        });

    return expect(lookupCalls == 1 && lookedUpId == session.id, "existing lookup by character ID") &&
           expect(touchCalls == 1, "existing session touched once") &&
           expect(createCalls == 0, "existing session not created pending");
}

auto testMissingSessionCreatesPendingOnly() -> bool
{
    uint32 lookedUpId{};
    uint32 createdId{};
    int    lookupCalls{};
    int    touchCalls{};
    int    createCalls{};

    mapipc::HandleCharZone(
        ipc::CharZone{ .charId = 0, .destinationZoneId = 230 },
        [&](const uint32 charId) -> FakeSession*
        {
            ++lookupCalls;
            lookedUpId = charId;
            return nullptr;
        },
        [&](FakeSession*)
        {
            ++touchCalls;
        },
        [&](const uint32 charId)
        {
            ++createCalls;
            createdId = charId;
        });

    return expect(lookupCalls == 1 && lookedUpId == 0, "zero character ID looked up") &&
           expect(touchCalls == 0, "missing session not touched") &&
           expect(createCalls == 1 && createdId == 0, "zero character ID created pending");
}

auto testForcedSessionTapGuard() -> bool
{
    MapSession session{};
    session.forceLinkDead = true;
    session.tapLastUpdate();
    const auto forcedTime = session.last_update;

    session.forceLinkDead = false;
    session.tapLastUpdate();

    return expect(forcedTime == earth_time::time_point{}, "forced session timestamp unchanged") &&
           expect(session.last_update != earth_time::time_point{}, "active session timestamp refreshed");
}

} // namespace

auto runMapCharZone1254SelfTests() -> bool
{
    return testExistingSessionIsTouchedOnly() && testMissingSessionCreatesPendingOnly() && testForcedSessionTapGuard();
}
