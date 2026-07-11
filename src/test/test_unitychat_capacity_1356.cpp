#include "test_unitychat_capacity_1356.h"

#include "map/unitychat_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "unitychat capacity 1356 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runUnityChatCapacity1356SelfTests() -> bool
{
    bool ok = true;

    ok = expect(unitychathelpers::DelMemberRemaining(1), "remaining") && ok;
    ok = expect(!unitychathelpers::DelMemberRemaining(0), "empty") && ok;

    ok = expect(unitychathelpers::ShouldReceiveUnityPacket(false, false, false), "recv") && ok;
    ok = expect(!unitychathelpers::ShouldReceiveUnityPacket(true, false, false), "sender") && ok;
    ok = expect(!unitychathelpers::ShouldReceiveUnityPacket(false, true, false), "disappear") && ok;
    ok = expect(!unitychathelpers::ShouldReceiveUnityPacket(false, false, true), "prison") && ok;

    ok = expect(unitychathelpers::ShouldUnloadUnityChat(true), "unload") && ok;
    ok = expect(!unitychathelpers::ShouldUnloadUnityChat(false), "no unload") && ok;

    ok = expect(unitychathelpers::ShouldRejectNullOnlineMember(true), "null") && ok;
    ok = expect(unitychathelpers::FormatOnlineMemberNullWarning() == "PChar is null.", "null warn") && ok;

    ok = expect(unitychathelpers::ShouldLoadUnityChatOnOnlineAdd(false, 5), "load miss non-zero") && ok;
    ok = expect(!unitychathelpers::ShouldLoadUnityChatOnOnlineAdd(true, 5), "cache hit") && ok;
    ok = expect(!unitychathelpers::ShouldLoadUnityChatOnOnlineAdd(false, 0), "leader zero skip") && ok;

    ok = expect(unitychathelpers::ShouldAddMemberAfterOnlineLookup(true), "add") && ok;
    ok = expect(!unitychathelpers::OnlineMemberAlwaysReturnsFalse(), "always false") && ok;
    ok = expect(unitychathelpers::ShouldEraseUnityChatAfterDelOnline(false), "erase") && ok;
    ok = expect(!unitychathelpers::ShouldEraseUnityChatAfterDelOnline(true), "keep") && ok;
    ok = expect(unitychathelpers::ShouldReturnCachedUnityChat(true), "get") && ok;
    ok = expect(unitychathelpers::FormatDelOnlineMemberException("boom") ==
                    "unitychat::DelOnlineMember caught exception: boom",
                "exc") &&
         ok;

    return ok;
}
