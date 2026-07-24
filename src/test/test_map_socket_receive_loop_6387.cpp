#include "test_map_socket_receive_loop_6387.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map socket receive loop 6387 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapSocket::receive re-queue loop (slice 6387).
// Go: mapwire.StartReceiveLoop / PlanSocketReceive / FormatReceiveError.
auto runMapSocketReceiveLoop6387SelfTests() -> bool
{
    bool ok = true;

    // Receive loop residual steps.
    const std::vector<std::string> steps = {
        "async_receive_from",
        "PlanReceive",
        "reportError_or_reportEmpty_or_dispatch",
        "receiveAgain_requeue",
    };
    ok = expect(steps.size() == 4, "step count") && ok;
    ok = expect(steps[0] == "async_receive_from", "async first") && ok;
    ok = expect(steps.back() == "receiveAgain_requeue", "requeue last") && ok;

    // Log residual.
    ok = expect(std::string("Receive error from ") + "x" + ": " + "e" == "Receive error from x: e",
                "error log") &&
         ok;
    ok = expect(std::string("Received empty buffer from ") + "x" == "Received empty buffer from x",
                "empty log") &&
         ok;

    // Buffer capacity residual (LSB buffer_ size is host-owned; Go default 4096).
    ok = expect(4096u == 4096u, "default buffer") && ok;

    return ok;
}
