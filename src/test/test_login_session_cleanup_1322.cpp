#include "test_login_session_cleanup_1322.h"

#include "common/cbasetypes.h"
#include "login/session_cleanup.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login session cleanup 1322 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlan(
    const bool entryFound,
    const loginHelpers::session_error_peer peer,
    const bool otherPeerPresent,
    const bool wantClear,
    const bool wantErase,
    const char* label) -> bool
{
    const auto plan = loginHelpers::PlanSessionErrorCleanup(entryFound, peer, otherPeerPresent);
    return expect(plan.clearPeer == wantClear && plan.eraseSessionEntry == wantErase, label);
}

} // namespace

auto runLoginSessionCleanup1322SelfTests() -> bool
{
    using peer = loginHelpers::session_error_peer;
    bool ok    = true;

    ok = expect(!loginHelpers::HasSessionHashForCleanup(""), "empty hash") && ok;
    ok = expect(loginHelpers::HasSessionHashForCleanup("abc"), "non-empty hash") && ok;

    // Entry not found: no clear, no erase (both peers irrelevant).
    ok = expectPlan(false, peer::VIEW, false, false, false, "view not found") && ok;
    ok = expectPlan(false, peer::DATA, true, false, false, "data not found") && ok;

    // Entry found, other peer present: clear erroring peer only.
    ok = expectPlan(true, peer::VIEW, true, true, false, "view clear keep data") && ok;
    ok = expectPlan(true, peer::DATA, true, true, false, "data clear keep view") && ok;

    // Entry found, other peer absent: clear and erase session entry.
    ok = expectPlan(true, peer::VIEW, false, true, true, "view clear erase") && ok;
    ok = expectPlan(true, peer::DATA, false, true, true, "data clear erase") && ok;

    // Peer kind does not change the plan (symmetric).
    const auto viewPlan = loginHelpers::PlanSessionErrorCleanup(true, peer::VIEW, false);
    const auto dataPlan = loginHelpers::PlanSessionErrorCleanup(true, peer::DATA, false);
    ok = expect(viewPlan.clearPeer == dataPlan.clearPeer && viewPlan.eraseSessionEntry == dataPlan.eraseSessionEntry,
                "peer kind symmetric") &&
         ok;

    ok = expect(loginHelpers::ShouldEraseIPAfterSessionErase(true), "ip empty erase") && ok;
    ok = expect(!loginHelpers::ShouldEraseIPAfterSessionErase(false), "ip non-empty keep") && ok;

    ok = expect(loginHelpers::FormatCharacterCreatedInfo("Alice", 1001) ==
                    "char <Alice> was successfully created on account 1001",
                "create info") &&
         ok;
    ok = expect(loginHelpers::FormatCharacterCreatedInfo("", 0) ==
                    "char <> was successfully created on account 0",
                "create info empty") &&
         ok;
    ok = expect(loginHelpers::FormatCharacterCreatedInfo("Bob", std::numeric_limits<uint32>::max()) ==
                    "char <Bob> was successfully created on account 4294967295",
                "create info max account") &&
         ok;

    ok = expect(loginHelpers::FormatNewCharacterNameError("Al1ce", "Invalid characters present in name.") ==
                    "new character name error <Al1ce>: Invalid characters present in name.",
                "name error") &&
         ok;
    ok = expect(loginHelpers::FormatNewCharacterNameError("X", "Invalid name length.") ==
                    "new character name error <X>: Invalid name length.",
                "name error length") &&
         ok;

    return ok;
}
