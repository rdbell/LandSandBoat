#include "test_aicontainer_internal_die_synth_useitem_host_6370.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal Die/Synth/UseItem host 6370 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Internal_Die / Internal_Synth / Internal_UseItem
// admission residuals (slice 6370). Go hosts compose these gates with
// ChangeState / ForceChangeState on Stack.
auto runAicontainerInternalDieSynthUseItemHost6370SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanPushState;
    using aicontainerhelpers::InternalDieHasBattleEntity;
    using aicontainerhelpers::InternalSynthAllowed;
    using aicontainerhelpers::InternalUseItemHasCharEntity;
    using aicontainerhelpers::ShouldCheckActionQueue;

    bool ok = true;

    // Internal_Die outer gate (6298).
    ok = expect(InternalDieHasBattleEntity(true), "die battle") && ok;
    ok = expect(!InternalDieHasBattleEntity(false), "die no battle") && ok;

    // Internal_Synth admission (6304).
    ok = expect(InternalSynthAllowed(true, false), "synth char not synth") && ok;
    ok = expect(!InternalSynthAllowed(true, true), "synth already") && ok;
    ok = expect(!InternalSynthAllowed(false, false), "synth no char") && ok;

    // Internal_UseItem outer gate (6307).
    ok = expect(InternalUseItemHasCharEntity(true), "useitem char") && ok;
    ok = expect(!InternalUseItemHasCharEntity(false), "useitem no char") && ok;

    // ChangeState residuals used by OnStack composition (6368).
    ok = expect(CanPushState(10), "push ceiling") && ok;
    ok = expect(CanChangeState(false, false), "idle can change") && ok;
    ok = expect(!CanChangeState(true, false), "current blocks change") && ok;

    // Tick residual still holds (6369).
    ok = expect(ShouldCheckActionQueue(true), "6369 residual") && ok;

    return ok;
}
