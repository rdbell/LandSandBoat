#include "test_login_handler_session_peer_2749.h"

#include "login/handler_session_peer.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login handler-session peer 2749 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginHandlerSessionPeer2749SelfTests() -> bool
{
    const auto connected = planHandlerSessionPeer("192.0.2.7", false);
    const auto failed    = planHandlerSessionPeer("ignored", true);

    return expect(connected.enableReuseAddress, "connected enables reuse address") &&
           expect(connected.ipAddress == "192.0.2.7", "connected records remote address") &&
           expect(!connected.closeSocket, "connected keeps socket open") &&
           expect(failed.enableReuseAddress, "failed lookup still enables reuse address") &&
           expect(failed.ipAddress == "error", "failed lookup records error") &&
           expect(failed.closeSocket, "failed lookup closes socket");
}
