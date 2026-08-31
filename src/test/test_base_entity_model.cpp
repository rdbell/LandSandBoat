#include "test_base_entity_model.h"

#include "map/entities/char_entity.h"
#include "map/entities/fellow_entity.h"
#include "map/zone.h"
#include "omega_self_test_registry.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "base-entity model self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFloat(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "base-entity model self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testIdentityPositionAndDynamicState() -> bool
{
    CCharEntity entity;
    entity.id           = 0x11223344;
    entity.targid       = 0x06FF;
    entity.loc.destination = 241;
    entity.loc.p        = position_t{ 1.25f, -2.5f, 3.75f, 9, 32 };

    bool ok = true;
    ok      = expect(entity.id == 0x11223344 && entity.targid == 0x06FF, "identity fields") && ok;
    ok      = expect(entity.getZone() == 241, "destination zone fallback") && ok;
    ok      = expectFloat(entity.GetXPos(), 1.25f, "x position") && ok;
    ok      = expectFloat(entity.GetYPos(), -2.5f, "y position") && ok;
    ok      = expectFloat(entity.GetZPos(), 3.75f, "z position") && ok;
    ok      = expect(entity.GetRotPos() == 32, "rotation") && ok;
    ok      = expect(!entity.IsDynamicEntity(), "static target-id classification") && ok;

    entity.targid = 0x0700;
    ok            = expect(entity.IsDynamicEntity(), "dynamic target-id classification") && ok;
    return ok;
}

auto testDynamicEntityClassification8600() -> bool
{
    CCharEntity entity;
    bool ok = true;

    for (const auto targid : { uint16{ 0x0000 }, uint16{ 0x06FF } })
    {
        entity.targid = targid;
        ok = expect(!entity.IsDynamicEntity(), "static target-id is not dynamic") && ok;
    }
    for (const auto targid : { uint16{ 0x0700 }, uint16{ 0xFFFF } })
    {
        entity.targid = targid;
        ok = expect(entity.IsDynamicEntity(), "dynamic target-id classification") && ok;
    }
    return ok;
}

auto testCharacterConfigurationPredicates() -> bool
{
    CCharEntity entity;

    bool ok = true;
    ok      = expect(entity.isNewPlayer(), "zero config is new player") && ok;
    ok      = expect(!entity.isSeekingParty(), "zero config is not seeking") && ok;
    ok      = expect(!entity.isAnon(), "zero config is not anonymous") && ok;
    ok      = expect(!entity.isAway(), "zero config is not away") && ok;
    ok      = expect(entity.hasAutoTargetEnabled(), "zero config enables auto target") && ok;

    entity.playerConfig.NewAdventurerOffFlg = 1;
    entity.playerConfig.InviteFlg           = 1;
    entity.playerConfig.AnonymityFlg        = 1;
    entity.playerConfig.AwayFlg             = 1;
    entity.playerConfig.AutoTargetOffFlg    = 1;

    ok = expect(!entity.isNewPlayer(), "new-adventurer off flag") && ok;
    ok = expect(entity.isSeekingParty(), "invite flag") && ok;
    ok = expect(entity.isAnon(), "anonymity flag") && ok;
    ok = expect(entity.isAway(), "away flag") && ok;
    ok = expect(!entity.hasAutoTargetEnabled(), "auto-target off flag") && ok;
    return ok;
}

auto testFellowConstructorClassification() -> bool
{
    CFellowEntity entity(nullptr);
    return expect(entity.objtype == TYPE_FELLOW, "fellow constructor classification");
}

auto testZoneLineSpawnCycle() -> bool
{
    zoneLine_t line{};
    line.destinationPos    = position_t{ 10.0f, 2.0f, 20.0f, 0, 0 };
    line.destinationScaleX = 2.0f;
    line.destinationScaleZ = 9.0f;

    bool ok = true;
    for (uint8 slot = 0; slot < 8; ++slot)
    {
        const auto position = line.nextSpawnPosition();
        ok = expectFloat(position.x, 10.0f, "zoneline spawn x") && ok;
        ok = expectFloat(position.y, 2.0f, "zoneline spawn y") && ok;
        ok = expectFloat(position.z, 16.0f + slot, "zoneline spawn z") && ok;
    }

    const auto wrapped = line.nextSpawnPosition();
    ok                 = expectFloat(wrapped.z, 16.0f, "zoneline wrapped spawn z") && ok;
    return ok;
}

} // namespace

auto runBaseEntityModelSelfTests() -> bool
{
    return testIdentityPositionAndDynamicState() &&
           testCharacterConfigurationPredicates() &&
           testFellowConstructorClassification() &&
           testZoneLineSpawnCycle();
}

OMEGA_REGISTER_SELF_TEST("entity-dynamic-8600", testDynamicEntityClassification8600);
