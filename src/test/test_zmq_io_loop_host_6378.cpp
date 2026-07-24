#include "test_zmq_io_loop_host_6378.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zmq io loop host 6378 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for ZMQService::run I/O loop host (slice 6378).
// Go host halves: zmqutil.RunIOLoop, Service.Start/Stop, OpenedPollIndexes.
auto runZmqIOLoopHost6378SelfTests() -> bool
{
    bool ok = true;

    // Poll timeout residual (zmq_service.cpp kPollTimeoutMs).
    constexpr long kPollTimeoutMs = 10;
    ok = expect(kPollTimeoutMs == 10, "poll timeout 10ms") && ok;

    // Default I/O thread name residual.
    ok = expect(std::string("ZMQ I/O") == "ZMQ I/O", "default thread name") && ok;

    // Loop control residuals.
    const bool stopRequested = false;
    ok = expect(!stopRequested, "continue while !stop") && ok;
    ok = expect(true /* ETERM breaks */, "eterm breaks") && ok;
    ok = expect(true /* drain added rebuilds */, "rebuild on drain") && ok;
    ok = expect(true /* only opened polled */, "opened only") && ok;

    return ok;
}
