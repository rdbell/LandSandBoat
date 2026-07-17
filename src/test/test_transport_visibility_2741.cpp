#include "test_transport_visibility_2741.h"

#include <iostream>

#include "map/transport.h"

auto runTransportVisibility2741SelfTests() -> bool
{
    const auto visible = transporthelpers::VisibilityFor(true);
    const auto hidden  = transporthelpers::VisibilityFor(false);
    const bool ok      = visible.status == static_cast<uint8>(STATUS_TYPE::NORMAL) && visible.moving == 0x8007 &&
                     hidden.status == static_cast<uint8>(STATUS_TYPE::DISAPPEAR) && hidden.moving == 0x8006;
    if (!ok)
    {
        std::cerr << "transport visibility self-test failed\n";
    }
    return ok;
}
