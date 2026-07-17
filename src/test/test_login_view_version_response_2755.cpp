#include "test_login_view_version_response_2755.h"

#include "login/view_version_response.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login view version response 2755 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginViewVersionResponse2755SelfTests() -> bool
{
    const auto normalWithPeer = login::PlanViewVersionResponse(login::version_lock::ResponseLength::KeyPacket, true);
    const auto normalNoPeer   = login::PlanViewVersionResponse(login::version_lock::ResponseLength::KeyPacket, false);
    const auto fatalWithPeer  = login::PlanViewVersionResponse(login::version_lock::ResponseLength::VersionError, true);
    const auto fatalNoPeer    = login::PlanViewVersionResponse(login::version_lock::ResponseLength::VersionError, false);

    return expect(normalWithPeer.shapeKeyPacket && normalWithPeer.writeKeyPacket && !normalWithPeer.writeVersionError && !normalWithPeer.returnFromRead, "normal peer writes key") &&
           expect(normalNoPeer.shapeKeyPacket && !normalNoPeer.writeKeyPacket && !normalNoPeer.writeVersionError && !normalNoPeer.returnFromRead, "normal missing peer shapes only") &&
           expect(!fatalWithPeer.shapeKeyPacket && !fatalWithPeer.writeKeyPacket && fatalWithPeer.writeVersionError && fatalWithPeer.returnFromRead, "fatal peer writes error and returns") &&
           expect(fatalNoPeer.shapeKeyPacket && !fatalNoPeer.writeKeyPacket && !fatalNoPeer.writeVersionError && !fatalNoPeer.returnFromRead, "fatal missing peer falls through to key shaping");
}
