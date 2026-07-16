#include "test_login_authenticated_session_registry_2312.h"

#include "login/login_helpers.h"
#include "login/session_cleanup.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login authenticated-session registry 2312 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginAuthenticatedSessionRegistry2312SelfTests() -> bool
{
    constexpr const char* ipA   = "registry-2312-a";
    constexpr const char* ipB   = "registry-2312-b";
    constexpr const char* hashA = "hash-a";
    constexpr const char* hashB = "hash-b";

    // This suite removes every entry it creates, so it composes with the
    // process-wide production registry used by the login hosts.
    loginHelpers::erase_authenticated_session(ipA, hashA);
    loginHelpers::erase_authenticated_session(ipA, hashB);
    loginHelpers::erase_authenticated_session(ipB, hashA);

    bool ok = true;
    const auto countBefore   = loginHelpers::authenticated_session_count();
    const auto ipCountBefore = loginHelpers::authenticated_ip_count();

    auto& first       = loginHelpers::get_authenticated_session(ipA, hashA);
    first.accountID   = 42;
    first.serverIP    = 0x01020304;
    first.versionMismatch = true;
    ok = expect(loginHelpers::authenticated_session_count() == countBefore + 1, "create count") && ok;

    auto& reused = loginHelpers::get_authenticated_session(ipA, hashA);
    ok = expect(&first == &reused && reused.accountID == 42 && reused.serverIP == 0x01020304 && reused.versionMismatch,
                "reuse preserves state") &&
         ok;

    auto& sameIPDifferentHash = loginHelpers::get_authenticated_session(ipA, hashB);
    sameIPDifferentHash.accountID = 43;
    auto& sameHashDifferentIP = loginHelpers::get_authenticated_session(ipB, hashA);
    sameHashDifferentIP.accountID = 44;
    ok = expect(loginHelpers::authenticated_session_count() == countBefore + 3, "ip hash separation count") && ok;
    ok = expect(loginHelpers::find_authenticated_session(ipA, hashB)->accountID == 43 &&
                    loginHelpers::find_authenticated_session(ipB, hashA)->accountID == 44,
                "ip hash separation values") &&
         ok;

    ok = expect(loginHelpers::find_authenticated_session(ipA, "unknown") == nullptr, "unknown lookup") && ok;
    ok = expect(loginHelpers::find_authenticated_session("unknown", hashA) == nullptr, "unknown ip lookup") && ok;
    ok = expect(loginHelpers::authenticated_session_count() == countBefore + 3, "lookup does not create") && ok;
    ok = expect(!loginHelpers::erase_authenticated_session("unknown", "unknown"), "unknown removal") && ok;

    // The registry accepts empty keys exactly as operator[] did; an empty
    // lookup is still non-constructing and an empty entry remains removable.
    ok = expect(loginHelpers::find_authenticated_session("", "") == nullptr, "empty lookup") && ok;
    loginHelpers::get_authenticated_session("", "").accountID = 45;
    ok = expect(loginHelpers::find_authenticated_session("", "")->accountID == 45, "empty create") && ok;
    ok = expect(loginHelpers::erase_authenticated_session("", ""), "empty removal") && ok;

    // Both handler error paths use this plan before calling the registry erase:
    // keep the entry while the other peer is present, erase it otherwise.
    const auto keepPlan = loginHelpers::PlanSessionErrorCleanup(true, loginHelpers::session_error_peer::VIEW, true);
    const auto erasePlan = loginHelpers::PlanSessionErrorCleanup(true, loginHelpers::session_error_peer::DATA, false);
    ok = expect(keepPlan.clearPeer && !keepPlan.eraseSessionEntry, "peer present keeps entry") && ok;
    ok = expect(erasePlan.clearPeer && erasePlan.eraseSessionEntry, "no peer erases entry") && ok;

    ok = expect(loginHelpers::erase_authenticated_session(ipA, hashA), "remove first") && ok;
    ok = expect(loginHelpers::find_authenticated_session(ipA, hashA) == nullptr, "removed lookup") && ok;
    ok = expect(loginHelpers::erase_authenticated_session(ipA, hashB), "remove final ip entry") && ok;
    ok = expect(loginHelpers::authenticated_ip_count() == ipCountBefore + 1, "outer map cleanup") && ok;
    ok = expect(loginHelpers::erase_authenticated_session(ipB, hashA), "remove other ip") && ok;
    ok = expect(loginHelpers::authenticated_session_count() == countBefore, "cleanup restores count") && ok;
    ok = expect(loginHelpers::authenticated_ip_count() == ipCountBefore, "cleanup restores ip count") && ok;

    return ok;
}
