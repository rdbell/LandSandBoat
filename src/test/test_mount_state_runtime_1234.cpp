#include "test_mount_state_runtime_1234.h"

#include "../map/entities/base_entity.h"
#include "../map/status_effect.h"
#include "../map/utils/mountutils.h"

#include <chrono>
#include <cstdio>

namespace
{
auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::fprintf(stderr, "mount state runtime: %s\n", label);
    }
    return condition;
}
} // namespace

auto runMountStateRuntime1234SelfTests() -> bool
{
    using namespace std::chrono_literals;

    auto ok = true;
    auto absent = mountutils::resolveState(nullptr);
    ok = expect(!absent.mounted && absent.animation == MountAnimation::None, "null effect is absent") && ok;

    CStatusEffect chocobo(xi::StatusEffect::Mounted, 0, MOUNT_CHOCOBO, 0s, 30min, 0, 17);
    auto          chocoboState = mountutils::resolveState(&chocobo);
    ok = expect(chocoboState.mounted && chocoboState.mount == MOUNT_CHOCOBO, "chocobo resolves") && ok;
    ok = expect(chocoboState.subPower == 17 && chocoboState.animation == MountAnimation::Chocobo, "chocobo metadata") && ok;

    CStatusEffect noble(xi::StatusEffect::Mounted, 0, MOUNT_NOBLE_CHOCOBO, 0s, 30min);
    ok = expect(mountutils::resolveState(&noble).animation == MountAnimation::Chocobo, "noble animation") && ok;

    CStatusEffect tiger(xi::StatusEffect::Mounted, 0, MOUNT_TIGER, 0s, 30min);
    ok = expect(mountutils::resolveState(&tiger).animation == MountAnimation::Mount, "generic mount animation") && ok;

    CStatusEffect sentinel(xi::StatusEffect::Mounted, 0, MOUNT_MAX, 0s, 30min, 0, 99);
    auto          sentinelState = mountutils::resolveState(&sentinel);
    ok = expect(sentinelState.mounted && sentinelState.mount == MOUNT_MAX && sentinelState.subPower == 99,
                "sentinel follows generic mount path") && ok;

    CStatusEffect wide(xi::StatusEffect::Mounted, 0, UINT16_MAX, 0s, 30min);
    ok = expect(mountutils::resolveState(&wide).mounted && mountutils::resolveState(&wide).mount == UINT16_MAX,
                "wide power follows generic mount path") && ok;
    return ok;
}
