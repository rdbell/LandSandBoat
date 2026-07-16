#include "test_s2c_wpos_runtime.h"
#include "map/packets/s2c/wpos_runtime.h"

auto runS2CWPosRuntimeSelfTests() -> bool
{
    position_t p{};
    p.x        = 1;
    p.y        = 2;
    p.z        = 3;
    p.rotation = 4;
    position_t q{};
    q.x           = 5;
    q.y           = 6;
    q.z           = 7;
    q.rotation    = 8;
    auto normal   = wposhelpers::PlanFor({ .current = p, .character = true, .locked = true }, q, POSMODE::NORMAL);
    auto reset    = wposhelpers::PlanFor({ .current = p, .character = true, .locked = true }, q, POSMODE::RESET);
    auto rotate   = wposhelpers::PlanFor({ .current = p, .character = true, .locked = true }, q, POSMODE::ROTATE);
    auto lock     = wposhelpers::PlanFor({ .current = p, .character = false, .locked = false }, q, POSMODE::LOCK);
    auto charLock = wposhelpers::PlanFor({ .current = p, .character = true, .locked = false }, q, POSMODE::LOCK);
    auto unlock   = wposhelpers::PlanFor({ .current = p, .character = true, .locked = true }, q, POSMODE::UNLOCK);
    return normal.current.x == 5 && normal.current.rotation == 8 && reset.locked == false && rotate.current.x == 1 && rotate.current.rotation == 8 && !lock.locked && charLock.locked && !unlock.locked;
}
