#include "test_despawn_respawn_registration_6287.h"

#include "map/ai/states/despawn_respawn_registration.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "despawn respawn registration 6287 self-test failed: " << label << '\n';
    }

    return condition;
}
} // namespace

auto runDespawnRespawnRegistration6287SelfTests() -> bool
{
    using despawnrespawnregistration::shouldRegister;

    return expect(shouldRegister(true, true, true), "respawnable zoned mob registers") &&
           expect(!shouldRegister(false, true, true), "non-mob does not register") &&
           expect(!shouldRegister(true, false, true), "respawn-disabled mob does not register") &&
           expect(!shouldRegister(true, true, false), "zoneless mob does not register");
}
