#include "test_mount_dismount_2852.h"

#include "map/utils/mount_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mount dismount 2852 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameDecision(const mountutilshelpers::DismountDecision& a, const mountutilshelpers::DismountDecision& b) -> bool
{
    return a.removeStatus == b.removeStatus && a.animation == b.animation && a.updateHP == b.updateHP;
}

} // namespace

auto runMountDismount2852SelfTests() -> bool
{
    using mountutilshelpers::DismountDecision;
    using mountutilshelpers::PlanDismount;

    bool ok = true;

    // --- !mounted → empty Decision ---
    {
        const auto got  = PlanDismount(false);
        const auto want = DismountDecision{};
        ok              = expect(sameDecision(got, want), "not mounted empty") && ok;
        ok              = expect(!got.removeStatus && !got.updateHP, "not mounted flags clear") && ok;
        ok              = expect(got.animation == MountAnimation::None, "not mounted animation None") && ok;
    }

    // --- mounted → RemoveStatus, AnimationNone, UpdateHP ---
    {
        const auto got  = PlanDismount(true);
        const auto want = DismountDecision{
            .removeStatus = true,
            .animation    = MountAnimation::None,
            .updateHP     = true,
        };
        ok = expect(sameDecision(got, want), "mounted full plan") && ok;
        ok = expect(got.removeStatus, "mounted removeStatus") && ok;
        ok = expect(got.animation == MountAnimation::None, "mounted animation None") && ok;
        ok = expect(got.updateHP, "mounted updateHP") && ok;
    }

    // --- Composition table: dual-wire pure decision from host-injected mounted ---
    const struct
    {
        bool        mounted;
        bool        wantRemove;
        bool        wantUpdateHP;
        const char* label;
    } compose[] = {
        { false, false, false, "compose not mounted" },
        { true, true, true, "compose mounted" },
    };
    for (const auto& c : compose)
    {
        const auto got = PlanDismount(c.mounted);
        ok             = expect(got.removeStatus == c.wantRemove, c.label) && ok;
        ok             = expect(got.updateHP == c.wantUpdateHP, c.label) && ok;
        ok             = expect(got.animation == MountAnimation::None, c.label) && ok;
        // Production Dismount action applies mutations only when removeStatus.
        ok = expect(got.removeStatus == c.mounted, "compose remove iff mounted") && ok;
    }

    return ok;
}
