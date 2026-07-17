#include "test_mount_packet_2844.h"

#include "map/utils/mount_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mount packet 2844 self-test failed: " << label << '\n';
    }
    return condition;
}

// Production MOUNTTYPE pins (base_entity.h) without pulling entity headers.
constexpr uint16_t MountChocobo      = 0;
constexpr uint16_t MountNobleChocobo = 34;
constexpr uint16_t MountTiger        = 3;
constexpr uint16_t MountMax          = 38;

auto sameDefinition(const MountPacketDefinition& a, const MountPacketDefinition& b) -> bool
{
    return a.ChocoboIndex == b.ChocoboIndex && a.CustomProperties[0] == b.CustomProperties[0] &&
           a.CustomProperties[1] == b.CustomProperties[1];
}

} // namespace

auto runMountPacket2844SelfTests() -> bool
{
    using mountutilshelpers::PlanMountPacketDefinition;

    bool ok = true;

    // --- !mounted → ChocoboIndex=0 (power / field ignored) ---
    {
        const auto got  = PlanMountPacketDefinition(false, MountChocobo, 0xDEADBEEF);
        const auto want = MountPacketDefinition{ .ChocoboIndex = 0 };
        ok              = expect(sameDefinition(got, want), "not mounted chocobo") && ok;

        const auto noble = PlanMountPacketDefinition(false, MountNobleChocobo, 0);
        ok               = expect(sameDefinition(noble, want), "not mounted noble") && ok;

        const auto tiger = PlanMountPacketDefinition(false, MountTiger, 1);
        ok               = expect(sameDefinition(tiger, want), "not mounted tiger") && ok;
    }

    // --- MOUNT_CHOCOBO regular (fieldChocobo == 0) → index 1 ---
    {
        const auto got  = PlanMountPacketDefinition(true, MountChocobo, 0);
        const auto want = MountPacketDefinition{ .ChocoboIndex = 1 };
        ok              = expect(sameDefinition(got, want), "regular chocobo") && ok;
    }

    // --- MOUNT_CHOCOBO + fieldChocobo → index 2, CustomProperties={field, 0} ---
    {
        constexpr uint32_t field = 0x34120449;
        const auto         got   = PlanMountPacketDefinition(true, MountChocobo, field);
        const auto         want  = MountPacketDefinition{
                     .ChocoboIndex     = 2,
                     .CustomProperties = { field, 0 },
        };
        ok = expect(sameDefinition(got, want), "field chocobo") && ok;
    }

    // --- MOUNT_NOBLE_CHOCOBO → index (34%8)+2 = 4, CustomProperties={0,1} ---
    {
        const auto got  = PlanMountPacketDefinition(true, MountNobleChocobo, 0xFFFFFFFF);
        const auto want = MountPacketDefinition{
            .ChocoboIndex     = 4,
            .CustomProperties = { 0, 1 },
        };
        ok = expect(sameDefinition(got, want), "noble chocobo") && ok;
        ok = expect(got.CustomProperties[0] == 0, "noble ignores fieldChocobo") && ok;
        ok = expect(static_cast<uint8_t>((MountNobleChocobo % 8) + 2) == 4, "noble index formula pin") && ok;
    }

    // --- default → index (mount%8)+1, zero CustomProperties ---
    {
        const struct
        {
            uint16_t    power;
            uint8_t     index;
            const char* label;
        } cases[] = {
            { 1, 2, "quest raptor" },  // 1%8+1
            { 2, 3, "raptor" },        // 2%8+1
            { MountTiger, 4, "tiger" }, // 3%8+1
            { 7, 8, "ram" },           // 7%8+1
            { 8, 1, "morbol" },        // 8%8+1
            { 10, 3, "fenrir" },       // 10%8+1
            { 33, 2, "byakko" },       // 33%8+1
            { 35, 4, "ixion" },        // 35%8+1
            { 37, 6, "cracklaw" },     // 37%8+1
            { MountMax, 7, "max" },    // 38%8+1
            { 100, 5, "arbitrary" },   // 100%8+1
            { 0xffff, 8, "wide" },     // 0xffff%8+1 = 7+1
        };
        for (const auto& c : cases)
        {
            const auto got = PlanMountPacketDefinition(true, c.power, 0xABCD);
            ok             = expect(got.ChocoboIndex == c.index, c.label) && ok;
            ok             = expect(got.CustomProperties[0] == 0 && got.CustomProperties[1] == 0, "default props zero") &&
                 ok;
        }
    }

    // --- Composition table: production dual-wire scalars ---
    const struct
    {
        bool        mounted;
        uint16_t    power;
        uint32_t    field;
        uint8_t     wantIndex;
        uint32_t    wantProp0;
        uint32_t    wantProp1;
        const char* label;
    } compose[] = {
        { false, MountChocobo, 0xDEADBEEF, 0, 0, 0, "compose not mounted" },
        { true, MountChocobo, 0, 1, 0, 0, "compose regular chocobo" },
        { true, MountChocobo, 0x01020408, 2, 0x01020408, 0, "compose field chocobo" },
        { true, MountNobleChocobo, 0xFFFFFFFF, 4, 0, 1, "compose noble" },
        { true, MountTiger, 0xABCD, 4, 0, 0, "compose tiger" },
        { true, MountMax, 1, 7, 0, 0, "compose max" },
        { true, 0xffff, 0xffffffff, 8, 0, 0, "compose wide" },
    };
    for (const auto& c : compose)
    {
        const auto got = PlanMountPacketDefinition(c.mounted, c.power, c.field);
        ok             = expect(got.ChocoboIndex == c.wantIndex, c.label) && ok;
        ok             = expect(got.CustomProperties[0] == c.wantProp0, c.label) && ok;
        ok             = expect(got.CustomProperties[1] == c.wantProp1, c.label) && ok;
    }

    return ok;
}
