#include "test_mount_resolve_2839.h"

#include "map/utils/mount_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mount resolve 2839 self-test failed: " << label << '\n';
    }
    return condition;
}

// Production MOUNTTYPE pins (base_entity.h) without pulling entity headers.
constexpr uint16_t MountChocobo      = 0;
constexpr uint16_t MountNobleChocobo = 34;
constexpr uint16_t MountTiger        = 3;
constexpr uint16_t MountMax          = 38;

auto sameResolution(const MountStateResolution& a, const MountStateResolution& b) -> bool
{
    return a.mounted == b.mounted && a.mount == b.mount && a.subPower == b.subPower && a.animation == b.animation;
}

} // namespace

auto runMountResolve2839SelfTests() -> bool
{
    using mountutilshelpers::IsChocoboMountPower;
    using mountutilshelpers::PlanResolveMountState;

    bool ok = true;

    // --- IsChocoboMountPower ---
    ok = expect(IsChocoboMountPower(MountChocobo, MountChocobo, MountNobleChocobo), "chocobo power") && ok;
    ok = expect(IsChocoboMountPower(MountNobleChocobo, MountChocobo, MountNobleChocobo), "noble power") && ok;
    ok = expect(!IsChocoboMountPower(MountTiger, MountChocobo, MountNobleChocobo), "tiger not chocobo") && ok;
    ok = expect(!IsChocoboMountPower(MountMax, MountChocobo, MountNobleChocobo), "max not chocobo") && ok;
    ok = expect(!IsChocoboMountPower(0xffff, MountChocobo, MountNobleChocobo), "wide not chocobo") && ok;
    // Injected IDs: classification uses host-supplied constants
    ok = expect(IsChocoboMountPower(99, 99, 100), "injected chocobo match") && ok;
    ok = expect(IsChocoboMountPower(100, 99, 100), "injected noble match") && ok;
    ok = expect(!IsChocoboMountPower(0, 99, 100), "injected mismatch") && ok;

    // --- PlanResolveMountState: null effect ---
    {
        const auto got  = PlanResolveMountState(true, 0, 0, false);
        const auto want = MountStateResolution{ false, 0, 0, MountAnimation::None };
        ok              = expect(sameResolution(got, want), "null via isChocoboPower") && ok;

        const auto gotIDs = PlanResolveMountState(true, 5, 9, MountChocobo, MountNobleChocobo);
        ok                = expect(sameResolution(gotIDs, want), "null via injected IDs ignores power") && ok;
    }

    // --- Chocobo / Noble / generic ---
    {
        const auto chocobo = PlanResolveMountState(false, MountChocobo, 17, MountChocobo, MountNobleChocobo);
        ok                 = expect(chocobo.mounted && chocobo.mount == MountChocobo, "chocobo mounted+mount") && ok;
        ok                 = expect(chocobo.subPower == 17 && chocobo.animation == MountAnimation::Chocobo, "chocobo meta") && ok;

        const auto noble = PlanResolveMountState(false, MountNobleChocobo, 9, true);
        ok               = expect(noble.mounted && noble.mount == MountNobleChocobo, "noble mounted") && ok;
        ok               = expect(noble.subPower == 9 && noble.animation == MountAnimation::Chocobo, "noble meta") && ok;

        const auto tiger = PlanResolveMountState(false, MountTiger, 4, MountChocobo, MountNobleChocobo);
        ok               = expect(tiger.mounted && tiger.mount == MountTiger, "tiger mounted") && ok;
        ok               = expect(tiger.subPower == 4 && tiger.animation == MountAnimation::Mount, "tiger meta") && ok;
    }

    // --- Wide / sentinel powers follow generic mount path ---
    {
        const auto sentinel = PlanResolveMountState(false, MountMax, 99, MountChocobo, MountNobleChocobo);
        ok                  = expect(sentinel.mounted && sentinel.mount == MountMax && sentinel.subPower == 99, "sentinel fields") && ok;
        ok                  = expect(sentinel.animation == MountAnimation::Mount, "sentinel animation") && ok;

        const auto wide = PlanResolveMountState(false, 0xffff, 1, false);
        ok              = expect(wide.mounted && wide.mount == 0xffff && wide.subPower == 1, "wide fields") && ok;
        ok              = expect(wide.animation == MountAnimation::Mount, "wide animation") && ok;
    }

    // --- Composition: 5-arg Plan dual-wires IsChocoboMountPower into 4-arg Plan ---
    const struct
    {
        bool        effectNull;
        uint16_t    power;
        uint16_t    subPower;
        bool        wantMounted;
        MountAnimation wantAnimation;
        const char* label;
    } cases[] = {
        { true, 0, 0, false, MountAnimation::None, "compose null" },
        { false, MountChocobo, 1, true, MountAnimation::Chocobo, "compose chocobo" },
        { false, MountNobleChocobo, 2, true, MountAnimation::Chocobo, "compose noble" },
        { false, MountTiger, 3, true, MountAnimation::Mount, "compose tiger" },
        { false, MountMax, 0, true, MountAnimation::Mount, "compose max" },
        { false, 0xffff, 0, true, MountAnimation::Mount, "compose wide" },
    };
    for (const auto& c : cases)
    {
        const bool isChocobo = !c.effectNull && IsChocoboMountPower(c.power, MountChocobo, MountNobleChocobo);
        const auto viaBool   = PlanResolveMountState(c.effectNull, c.power, c.subPower, isChocobo);
        const auto viaIDs    = PlanResolveMountState(c.effectNull, c.power, c.subPower, MountChocobo, MountNobleChocobo);
        ok                   = expect(sameResolution(viaBool, viaIDs), "compose bool == IDs") && ok;
        ok                   = expect(viaIDs.mounted == c.wantMounted, c.label) && ok;
        ok                   = expect(viaIDs.animation == c.wantAnimation, c.label) && ok;
        ok                   = expect(viaIDs.mount == (c.effectNull ? uint16_t{ 0 } : c.power), "compose mount field") && ok;
        ok                   = expect(viaIDs.subPower == (c.effectNull ? uint16_t{ 0 } : c.subPower), "compose subPower field") && ok;
    }

    // --- Injected IDs reclassify animation ---
    {
        const auto remapped = PlanResolveMountState(false, MountTiger, 0, MountTiger, 999);
        ok                  = expect(remapped.animation == MountAnimation::Chocobo, "injected tiger-as-chocobo") && ok;
        const auto normal   = PlanResolveMountState(false, MountTiger, 0, MountChocobo, MountNobleChocobo);
        ok                  = expect(normal.animation == MountAnimation::Mount, "production tiger mount anim") && ok;
    }

    return ok;
}
