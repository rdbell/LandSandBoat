#include "test_zone_packet_broadcast_6252.h"

#include "map/packet_broadcast_policy.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone packet broadcast 6252 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins CZoneEntities::PushPacket recipient policy, including its legacy
// entity-update spawn-list expression.
auto runZonePacketBroadcast6252SelfTests() -> bool
{
    using namespace zonepacketbroadcast;

    bool ok = true;

    for (const bool sourceCharacter : { false, true })
    {
        for (const bool hiddenGM : { false, true })
        {
            for (const uint8 flags : { uint8{ 0x00 }, EntityDespawnFlag })
            {
                const bool want = sourceCharacter && hiddenGM && flags != EntityDespawnFlag;
                ok              = expect(ShouldSuppressHiddenCharacterPacket(CharacterUpdatePacket, sourceCharacter, hiddenGM, flags) == want,
                             "hidden character update suppression") &&
                     ok;
            }
        }
    }
    ok = expect(!ShouldSuppressHiddenCharacterPacket(ActionPacket, true, true, 0), "only character updates are hidden-GM suppressed") && ok;

    for (const bool notSource : { false, true })
    {
        for (const bool inRange : { false, true })
        {
            for (const bool sameMogHouse : { false, true })
            {
                const bool want = notSource && inRange && sameMogHouse;
                ok              = expect(ShouldSendRangePacket(notSource, inRange, sameMogHouse) == want,
                             "range requires non-source, in-range, matching Mog House recipient") &&
                     ok;
                ok = expect(ShouldSendShoutPacket(notSource, inRange, sameMogHouse) == want,
                            "shout requires non-source, in-range, matching Mog House recipient") &&
                     ok;
            }
        }
    }

    for (const bool notInMogHouse : { false, true })
    {
        for (const bool notSource : { false, true })
        {
            ok = expect(ShouldSendZonePacket(notInMogHouse, notSource) == (notInMogHouse && notSource),
                        "zone requires a non-Mog-House non-source recipient") &&
                 ok;
        }
    }

    for (const uint8 flags : { uint8{ 0x00 }, EntityDespawnFlag, SpecialEntityFlag })
    {
        ok = expect(RequiresSpawnListFilter(EntityUpdatePacket, flags), "every entity update uses spawn-list filter") && ok;
    }
    ok = expect(RequiresSpawnListFilter(ActionPacket, 0), "action uses spawn-list filter") && ok;
    ok = expect(!RequiresSpawnListFilter(CharacterUpdatePacket, 0), "ordinary packet skips spawn-list filter") && ok;

    return ok;
}
