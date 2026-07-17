#include "test_login_view_world_list_response_2759.h"

#include "login/view_world_list_response.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login view world list response 2759 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginViewWorldListResponse2759SelfTests() -> bool
{
    const auto withPeer = login::PlanViewWorldListResponse(true);
    const auto noPeer   = login::PlanViewWorldListResponse(false);

    return expect(withPeer.shapeWorldListPacket && withPeer.writeWorldListPacket, "peer writes world list") &&
           expect(noPeer.shapeWorldListPacket && !noPeer.writeWorldListPacket, "missing peer shapes only");
}
