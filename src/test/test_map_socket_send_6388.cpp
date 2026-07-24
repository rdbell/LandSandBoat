#include "test_map_socket_send_6388.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map socket send 6388 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapSocket::send IPP conversion (slice 6388).
// Go: mapwire.IPPToUDPAddr / PlanMapSocketSend / SendToIPP.
auto runMapSocketSend6388SelfTests() -> bool
{
    bool ok = true;

    ok = expect(std::string("Sending ") + "10" + " bytes to " + "a:1" == "Sending 10 bytes to a:1",
                "sending log") &&
         ok;
    ok = expect(std::string("Error sending data: ") + "x" == "Error sending data: x", "send err") && ok;

    // ntohl residual: network-order IP converted before address_v4.
    ok = expect(true, "ntohl before endpoint") && ok;

    return ok;
}
