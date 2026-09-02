#include "test_avatars_favor_debuffs_9360.h"

#include "map/avatars_favor_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

auto runAvatarsFavorDebuffs9360SelfTests() -> bool
{
    using avatarsfavor::ShouldApplyPreSOADebuffs;

    bool ok = true;
    const auto expect = [&ok](const bool got, const bool want, const char* label) {
        if (got != want)
        {
            std::cerr << "avatars favor debuffs 9360 self-test failed: " << label << '\n';
            ok = false;
        }
    };

    // Lua: shouldAvatarsFavorBeApplied(petId) and ENABLE_SOA == 0.
    for (uint16_t petId = 8; petId <= 16; ++petId)
    {
        expect(ShouldApplyPreSOADebuffs(petId, false), true, "core avatar pre-SoA gate");
        expect(ShouldApplyPreSOADebuffs(petId, true), false, "core avatar SoA gate");
    }
    expect(ShouldApplyPreSOADebuffs(20, false), true, "Cait Sith pre-SoA gate");
    expect(ShouldApplyPreSOADebuffs(76, false), true, "Siren pre-SoA gate");

    for (const uint16_t petId : { uint16_t{ 0 }, uint16_t{ 7 }, uint16_t{ 17 }, uint16_t{ 19 }, uint16_t{ 21 }, uint16_t{ 75 }, uint16_t{ 77 }, uint16_t{ 255 } })
    {
        expect(ShouldApplyPreSOADebuffs(petId, false), false, "non-avatar excluded");
        expect(ShouldApplyPreSOADebuffs(petId, true), false, "non-avatar SoA excluded");
    }

    return ok;
}

OMEGA_REGISTER_SELF_TEST("avatars-favor-debuffs-9360", runAvatarsFavorDebuffs9360SelfTests);
