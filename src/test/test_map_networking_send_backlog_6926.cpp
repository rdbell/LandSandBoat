#include "test_map_networking_send_backlog_6926.h"

#include "map/map_networking_send_backlog.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking send backlog 6926 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingSendBacklog6926SelfTests() -> bool
{
    using mapnetworkingsendbackloghelpers::MakePlan;

    constexpr std::size_t maxBacklog = 192;
    bool                  ok         = true;

    auto plan = MakePlan(false, maxBacklog + 1, false, maxBacklog);
    ok = expect(!plan.reportRemainingPackets && !plan.warnUnzonedBacklog && !plan.warnZonedBacklog && !plan.clearPacketQueue,
                "disabled debug backlog does nothing") &&
         ok;

    plan = MakePlan(true, maxBacklog, true, maxBacklog);
    ok = expect(plan.reportRemainingPackets && !plan.warnUnzonedBacklog && !plan.warnZonedBacklog && !plan.clearPacketQueue,
                "debug backlog reports remaining packets at the limit") &&
         ok;

    plan = MakePlan(true, maxBacklog + 1, false, maxBacklog);
    ok = expect(plan.reportRemainingPackets && plan.warnUnzonedBacklog && !plan.warnZonedBacklog && plan.clearPacketQueue,
                "oversized unzoned backlog warns and clears") &&
         ok;

    plan = MakePlan(true, maxBacklog + 1, true, maxBacklog);
    ok = expect(plan.reportRemainingPackets && !plan.warnUnzonedBacklog && plan.warnZonedBacklog && !plan.clearPacketQueue,
                "oversized zoned backlog only warns") &&
         ok;

    return ok;
}
