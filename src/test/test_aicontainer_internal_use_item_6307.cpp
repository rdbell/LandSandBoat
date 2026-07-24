#include "test_aicontainer_internal_use_item_6307.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_UseItem 6307 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inlineUseItemHasChar(const bool hasCharEntity) -> bool
{
    return hasCharEntity;
}

auto pinUseItemHasChar(const bool hasCharEntity) -> bool
{
    return hasCharEntity;
}

} // namespace

// Pure dual-wire suite for Internal_UseItem outer char-entity gate
// (OmegaXI internal/aicontainer; slice 6307).
auto runAicontainerInternalUseItem6307SelfTests() -> bool
{
    using aicontainerhelpers::AcceptRaiseShouldInvoke;
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::CanFollowPath;
    using aicontainerhelpers::DespawnShouldDispatchController;
    using aicontainerhelpers::InternalActionTargetAllowed;
    using aicontainerhelpers::InternalDespawnAllowed;
    using aicontainerhelpers::InternalDieHasBattleEntity;
    using aicontainerhelpers::InternalSynthAllowed;
    using aicontainerhelpers::InternalUseItemHasCharEntity;

    bool ok = true;

    ok = expect(InternalUseItemHasCharEntity(true), "char entity → ChangeState + result") && ok;
    ok = expect(!InternalUseItemHasCharEntity(false), "no char entity → false") && ok;

    for (const bool hasChar : { false, true })
    {
        const bool got     = InternalUseItemHasCharEntity(hasChar);
        const bool inlineF = inlineUseItemHasChar(hasChar);
        const bool pinGot  = pinUseItemHasChar(hasChar);
        ok                 = expect(got == hasChar, "free==identity") && ok;
        ok                 = expect(got == inlineF, "free==inline") && ok;
        ok                 = expect(got == pinGot, "free==pin") && ok;
    }

    ok = expect(InternalSynthAllowed(true, false) && !InternalSynthAllowed(true, true),
                "6304 residual: synth admission still holds") &&
         ok;
    ok = expect(InternalDieHasBattleEntity(true) && !InternalDieHasBattleEntity(false),
                "6298 residual: die outer gate still holds") &&
         ok;
    ok = expect(InternalDespawnAllowed(false) && !InternalDespawnAllowed(true),
                "6300 residual: despawn admission still holds") &&
         ok;
    ok = expect(!InternalActionTargetAllowed(true, true) && InternalActionTargetAllowed(true, false),
                "6302 residual: action target gate still holds") &&
         ok;
    ok = expect(AcceptRaiseShouldInvoke(true) && !AcceptRaiseShouldInvoke(false),
                "6303 residual: accept raise still holds") &&
         ok;
    ok = expect(DespawnShouldDispatchController(true) && !DespawnShouldDispatchController(false),
                "6305 residual: despawn dispatch still holds") &&
         ok;
    ok = expect(CanFollowPath(true, false, false) && !CanFollowPath(false, false, true),
                "6306 residual: CanFollowPath still holds") &&
         ok;
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6307") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6307") &&
         ok;

    return ok;
}
