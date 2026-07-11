#include "test_map_entity_information_request_1292.h"

#include "map/entity_information_request.h"

#include <iostream>
#include <optional>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map entity information request 1292 self-test failed: " << label << '\n';
    }
    return condition;
}

struct Harness
{
    std::optional<mapipc::EntityInformationTarget>   character{};
    std::optional<mapipc::EntityInformationTarget>   entity{};
    std::optional<mapipc::EntityInformationPosition> spawn{};
    ipc::EntityInformationResponse                   response{};
    uint32                                           characterId{};
    uint32                                           entityId{};
    uint32                                           spawnId{};
    uint32                                           warningId{};
    int                                              characterCalls{};
    int                                              entityCalls{};
    int                                              spawnCalls{};
    int                                              sendCalls{};
    int                                              warningCalls{};

    void handle(const ipc::EntityInformationRequest& message)
    {
        mapipc::HandleEntityInformationRequest(
            message,
            [&](const uint32 id)
            {
                ++characterCalls;
                characterId = id;
                return character;
            },
            [&](const uint32 id)
            {
                ++entityCalls;
                entityId = id;
                return entity;
            },
            [&](const uint32 id)
            {
                ++spawnCalls;
                spawnId = id;
                return spawn;
            },
            [&](const ipc::EntityInformationResponse& value)
            {
                ++sendCalls;
                response = value;
            },
            [&](const uint32 id)
            {
                ++warningCalls;
                warningId = id;
            });
    }
};

auto testSpawnedPlayer() -> bool
{
    Harness harness{};
    harness.character = mapipc::EntityInformationTarget{
        .id         = 101,
        .objType    = TYPE_PC,
        .status     = STATUS_TYPE::NORMAL,
        .hasZone    = true,
        .zoneId     = 0xFFFE,
        .x          = -1.5f,
        .y          = 2.25f,
        .z          = 300.75f,
        .rot        = 0xFD,
        .moghouseId = 0xFFFFFFFE,
    };
    harness.handle(ipc::EntityInformationRequest{
        .requesterId = 0x10203040,
        .targetId    = 0xA0B0C0D0,
        .entityType  = TYPE_PC,
        .warp        = true,
        .spawnedOnly = true,
    });

    const auto& response = harness.response;
    return expect(harness.characterCalls == 1 && harness.characterId == 0xA0B0C0D0, "PC lookup") &&
           expect(harness.entityCalls == 0 && harness.spawnCalls == 0, "PC avoids other lookups") &&
           expect(harness.sendCalls == 1 && harness.warningCalls == 0, "PC response sent") &&
           expect(response.requesterId == 0x10203040 && response.targetId == 0xA0B0C0D0, "IDs preserved") &&
           expect(response.entityType == TYPE_PC && response.warp, "type and warp") &&
           expect(response.zoneId == 0xFFFE && response.x == -1.5f && response.y == 2.25f && response.z == 300.75f, "live location") &&
           expect(response.rot == 0xFD && response.moghouseId == 0xFFFFFFFE, "rotation and moghouse") &&
           expect(harness.character.has_value(), "spawnedOnly ignored");
}

auto testDespawnedMobUsesSpawnPoint() -> bool
{
    Harness harness{};
    harness.entity = mapipc::EntityInformationTarget{
        .id         = 0xDEADBEEF,
        .objType    = TYPE_MOB,
        .status     = STATUS_TYPE::DISAPPEAR,
        .hasZone    = true,
        .zoneId     = 77,
        .x          = 1.0f,
        .y          = 2.0f,
        .z          = 3.0f,
        .rot        = 99,
        .moghouseId = 88,
    };
    harness.spawn = mapipc::EntityInformationPosition{ -9.5f, 0.25f, 41.75f };
    harness.handle(ipc::EntityInformationRequest{
        .requesterId = 5,
        .targetId    = 6,
        .entityType  = TYPE_MOB,
        .warp        = true,
    });

    const auto& response = harness.response;
    return expect(harness.characterCalls == 0 && harness.entityCalls == 1 && harness.entityId == 6, "mob lookup") &&
           expect(harness.spawnCalls == 1 && harness.spawnId == 0xDEADBEEF, "spawn lookup uses entity ID") &&
           expect(response.x == -9.5f && response.y == 0.25f && response.z == 41.75f, "spawn location") &&
           expect(!response.warp, "despawned target cannot warp") &&
           expect(response.rot == 99 && response.moghouseId == 0, "rotation retained and non-PC moghouse cleared");
}

auto testMissingSpawnPointUsesZeroPosition() -> bool
{
    Harness harness{};
    harness.entity = mapipc::EntityInformationTarget{
        .id      = 9,
        .objType = TYPE_MOB,
        .status  = STATUS_TYPE::DISAPPEAR,
        .hasZone = true,
        .x       = 10.0f,
        .y       = 20.0f,
        .z       = 30.0f,
    };
    harness.handle(ipc::EntityInformationRequest{ .targetId = 9, .entityType = TYPE_MOB });

    return expect(harness.sendCalls == 1 && harness.spawnCalls == 1, "missing spawn still responds") &&
           expect(harness.response.x == 0.0f && harness.response.y == 0.0f && harness.response.z == 0.0f, "missing spawn zero position");
}

auto testSelectionUsesBitFlags() -> bool
{
    Harness harness{};
    harness.character = mapipc::EntityInformationTarget{
        .id      = 17,
        .objType = TYPE_PC,
        .status  = STATUS_TYPE::DISAPPEAR,
        .hasZone = true,
    };
    harness.spawn = mapipc::EntityInformationPosition{ 7.0f, 8.0f, 9.0f };
    harness.handle(ipc::EntityInformationRequest{ .targetId = 17, .entityType = TYPE_PC | TYPE_MOB, .warp = true });

    return expect(harness.characterCalls == 1 && harness.entityCalls == 0, "PC bit selects character") &&
           expect(harness.spawnCalls == 1, "mob bit selects spawn fallback") &&
           expect(harness.response.x == 7.0f && harness.response.moghouseId == 0, "combined flags response");
}

auto testMissingAndZonelessTargetsWarn() -> bool
{
    Harness missing{};
    missing.handle(ipc::EntityInformationRequest{ .targetId = 0xFFFFFFFF, .entityType = TYPE_PC });

    Harness zoneless{};
    zoneless.entity = mapipc::EntityInformationTarget{ .id = 44, .objType = TYPE_NPC, .hasZone = false };
    zoneless.handle(ipc::EntityInformationRequest{ .targetId = 45, .entityType = TYPE_NPC });

    return expect(missing.warningCalls == 1 && missing.warningId == 0xFFFFFFFF && missing.sendCalls == 0, "missing target warning") &&
           expect(zoneless.warningCalls == 1 && zoneless.warningId == 45 && zoneless.sendCalls == 0, "zoneless target warning") &&
           expect(missing.spawnCalls == 0 && zoneless.spawnCalls == 0, "rejected targets avoid spawn lookup");
}

} // namespace

auto runMapEntityInformationRequest1292SelfTests() -> bool
{
    return testSpawnedPlayer() && testDespawnedMobUsesSpawnPoint() && testMissingSpawnPointUsesZeroPosition() &&
           testSelectionUsesBitFlags() && testMissingAndZonelessTargetsWarn();
}
