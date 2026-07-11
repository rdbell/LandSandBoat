#include "test_map_player_relocation_1291.h"

#include "map/player_relocation.h"

#include <cstdint>
#include <iostream>

namespace
{

struct FakePosition
{
    float x{};
    float y{};
    float z{};
    uint16 moving{ 0xBEEF };
    uint8 rotation{};
};

struct FakeLocation
{
    FakePosition p{};
    uint16       destination{};
    uint16       boundary{ 9 };
    bool         zone{ true };
};

struct FakePlayer
{
    void clearPacketList()
    {
        ++clearCalls;
    }

    auto shouldPetPersistThroughZoning() const -> bool
    {
        ++petPersistenceChecks;
        return petPersists;
    }

    void setPetZoningInfo()
    {
        ++petZoningCalls;
    }

    FakeLocation loc{};
    uint32       m_moghouseID{};
    uint8        updatemask{ 0xFF };
    STATUS_TYPE  status{ STATUS_TYPE::UPDATE };
    uint8        animation{ 9 };
    bool         requestedWarp{};
    bool         requestedZoneChange{};
    bool         petPersists{};
    int          clearCalls{};
    mutable int  petPersistenceChecks{};
    int          petZoningCalls{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map player relocation 1291 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectRelocated(const FakePlayer& player, const bool warp, const bool petZoning) -> bool
{
    return expect(player.loc.p.x == -10.5f && player.loc.p.y == 0.25f && player.loc.p.z == 99.75f, "position") &&
           expect(player.loc.p.moving == 0xBEEF, "moving state preserved") &&
           expect(player.loc.p.rotation == 0xFE && player.loc.destination == 0xFFFF, "rotation and destination") &&
           expect(player.m_moghouseID == 0xFFFFFFFE, "moghouse ID") &&
           expect(player.loc.boundary == 0 && player.updatemask == 0, "boundary and update mask reset") &&
           expect(player.status == STATUS_TYPE::DISAPPEAR && player.animation == ANIMATION_NONE, "disappear and animation-none state") &&
           expect(player.clearCalls == 1, "packet list cleared once") &&
           expect(player.requestedWarp == warp && player.requestedZoneChange == !warp, "request flag") &&
           expect(player.petPersistenceChecks == 1, "pet persistence checked once") &&
           expect(player.petZoningCalls == (petZoning ? 1 : 0), "pet zoning state");
}

auto testRequestFlagsAreNotCleared() -> bool
{
    FakePlayer warp{};
    warp.requestedZoneChange = true;
    mapipc::ApplyPlayerRelocation(warp, mapipc::PlayerRelocation{}, mapipc::RelocationRequest::Warp);

    FakePlayer zoneChange{};
    zoneChange.requestedWarp = true;
    mapipc::ApplyPlayerRelocation(zoneChange, mapipc::PlayerRelocation{}, mapipc::RelocationRequest::ZoneChange);

    return expect(warp.requestedWarp && warp.requestedZoneChange, "warp preserves zone-change request") &&
           expect(zoneChange.requestedWarp && zoneChange.requestedZoneChange, "zone change preserves warp request");
}

auto testSendPlayerToLocation() -> bool
{
    FakePlayer player{};
    player.petPersists = true;
    uint32 lookedUpId{};
    int    lookups{};

    mapipc::HandleSendPlayerToLocation(
        ipc::SendPlayerToLocation{
            .targetId   = 0xABCDEF01,
            .zoneId     = 0xFFFF,
            .x          = -10.5f,
            .y          = 0.25f,
            .z          = 99.75f,
            .rot        = 0xFE,
            .moghouseId = 0xFFFFFFFE,
        },
        [&](const uint32 targetId)
        {
            ++lookups;
            lookedUpId = targetId;
            return &player;
        });

    return expect(lookups == 1 && lookedUpId == 0xABCDEF01, "send lookup uses target ID") &&
           expectRelocated(player, true, true);
}

auto testEntityInformationResponse() -> bool
{
    FakePlayer player{};
    uint32 lookedUpId{};
    int    lookups{};

    mapipc::HandleEntityInformationResponse(
        ipc::EntityInformationResponse{
            .requesterId = 0x10203040,
            .targetId    = 0xA0B0C0D0,
            .warp        = true,
            .zoneId      = 0xFFFF,
            .x           = -10.5f,
            .y           = 0.25f,
            .z           = 99.75f,
            .rot         = 0xFE,
            .moghouseId  = 0xFFFFFFFE,
        },
        [&](const uint32 requesterId)
        {
            ++lookups;
            lookedUpId = requesterId;
            return &player;
        });

    return expect(lookups == 1 && lookedUpId == 0x10203040, "response lookup uses requester ID") &&
           expectRelocated(player, false, false);
}

auto testRejectionCases() -> bool
{
    FakePlayer missingZone{};
    missingZone.loc.zone = false;
    mapipc::HandleSendPlayerToLocation(ipc::SendPlayerToLocation{}, [&](const uint32) { return &missingZone; });

    FakePlayer noWarp{};
    int        noWarpLookups{};
    mapipc::HandleEntityInformationResponse(
        ipc::EntityInformationResponse{ .warp = false },
        [&](const uint32)
        {
            ++noWarpLookups;
            return &noWarp;
        });

    int missingLookups{};
    mapipc::HandleSendPlayerToLocation(
        ipc::SendPlayerToLocation{},
        [&](const uint32) -> FakePlayer*
        {
            ++missingLookups;
            return nullptr;
        });

    return expect(missingZone.clearCalls == 0, "zoneless player unchanged") &&
           expect(noWarp.clearCalls == 0, "non-warp response unchanged") &&
           expect(noWarpLookups == 1, "non-warp response still performs lookup") &&
           expect(missingLookups == 1, "missing player looked up once");
}

} // namespace

auto runMapPlayerRelocation1291SelfTests() -> bool
{
    return testSendPlayerToLocation() && testEntityInformationResponse() && testRequestFlagsAreNotCleared() &&
           testRejectionCases();
}
