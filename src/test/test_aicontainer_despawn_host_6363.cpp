#include "test_aicontainer_despawn_host_6363.h"

#include "map/ai/ai_container_capacity.h"
#include "map/ai/controllers/controller_action_dispatch_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Despawn host 6363 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Despawn dispatch + Internal_Despawn admission (6363).
// Go host halves: aicontainer.Despawn / InternalDespawn, controller.Despawn.
auto runAicontainerDespawnHost6363SelfTests() -> bool
{
    using aicontainerhelpers::DespawnShouldDispatchController;
    using aicontainerhelpers::InternalDespawnAllowed;
    using aicontainerhelpers::ShouldCheckActionQueue;
    using controlleractiondispatch::Dispatch;

    bool ok = true;

    ok = expect(DespawnShouldDispatchController(true), "has controller") && ok;
    ok = expect(!DespawnShouldDispatchController(false), "no controller") && ok;
    ok = expect(InternalDespawnAllowed(false), "not despawning admits") && ok;
    ok = expect(!InternalDespawnAllowed(true), "already despawning rejects") && ok;

    // Controller Despawn owner gate (same Dispatch as Cast/Engage/...).
    int ownerCalls = 0;
    ok = expect(Dispatch(true, [&]() {
                    ++ownerCalls;
                    return true;
                }),
                "owner Dispatch true") &&
         ok;
    ok = expect(ownerCalls == 1, "owner Dispatch ran") && ok;
    ok = expect(!Dispatch(false, [&]() {
                    ++ownerCalls;
                    return true;
                }),
                "no owner Dispatch false") &&
         ok;
    ok = expect(ownerCalls == 1, "no owner did not run") && ok;

    // Compose: no controller → Internal path + admission.
    ok = expect(!DespawnShouldDispatchController(false) && InternalDespawnAllowed(false),
                "public no-ctrl → internal admits") &&
         ok;
    ok = expect(ShouldCheckActionQueue(true), "6362 residual") && ok;

    return ok;
}
