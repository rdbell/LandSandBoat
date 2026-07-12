#include "test_draw_in_1539.h"

#include "map/draw_in_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using drawinhelpers::DegreesToRadians;
using drawinhelpers::DrawInAfterSnapY;
using drawinhelpers::DrawInRaySourceY;
using drawinhelpers::EntityHeight;
using drawinhelpers::PostSnapYDelta;
using drawinhelpers::ShouldAbortDrawInNoZone;
using drawinhelpers::ShouldAbortDrawInRayBlock;
using drawinhelpers::ShouldApplyDrawInMove;

auto Check() -> bool
{
    if (std::fabs(DegreesToRadians(180.0f) - static_cast<float>(M_PI)) > 1e-5f)
    {
        return false;
    }
    if (DrawInRaySourceY(10.0f) != 8.0f || DrawInAfterSnapY(5.0f) != 4.0f)
    {
        return false;
    }
    if (!ShouldAbortDrawInNoZone(false) || ShouldAbortDrawInNoZone(true))
    {
        return false;
    }
    if (!ShouldAbortDrawInRayBlock(true) || ShouldAbortDrawInRayBlock(false))
    {
        return false;
    }
    if (!ShouldApplyDrawInMove(false, false, false) || ShouldApplyDrawInMove(true, false, false) ||
        ShouldApplyDrawInMove(false, true, false) || ShouldApplyDrawInMove(false, false, true))
    {
        return false;
    }
    if (EntityHeight != 2.0f || PostSnapYDelta != 1.0f)
    {
        return false;
    }
    return true;
}
} // namespace

auto runDrawIn1539SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "draw_in_1539 self-tests failed\n";
        return false;
    }
    return true;
}
