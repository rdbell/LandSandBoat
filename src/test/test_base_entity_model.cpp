#include "test_base_entity_model.h"

#include "map/entities/char_entity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "base-entity model self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectFloat(const float actual, const float expected, const char* label) -> bool
{
    if (std::fabs(actual - expected) > 0.0001f)
    {
        std::cerr << "base-entity model self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testIdentityPositionAndDynamicState() -> bool
{
    CCharEntity entity;
    entity.id           = 0x11223344;
    entity.targid       = 0x06FF;
    entity.loc.destination = 241;
    entity.loc.p        = position_t{ 1.25f, -2.5f, 3.75f, 9, 32 };

    bool ok = true;
    ok      = expect(entity.id == 0x11223344 && entity.targid == 0x06FF, "identity fields") && ok;
    ok      = expect(entity.getZone() == 241, "destination zone fallback") && ok;
    ok      = expectFloat(entity.GetXPos(), 1.25f, "x position") && ok;
    ok      = expectFloat(entity.GetYPos(), -2.5f, "y position") && ok;
    ok      = expectFloat(entity.GetZPos(), 3.75f, "z position") && ok;
    ok      = expect(entity.GetRotPos() == 32, "rotation") && ok;
    ok      = expect(!entity.IsDynamicEntity(), "static target-id classification") && ok;

    entity.targid = 0x0700;
    ok            = expect(entity.IsDynamicEntity(), "dynamic target-id classification") && ok;
    return ok;
}

} // namespace

auto runBaseEntityModelSelfTests() -> bool
{
    return testIdentityPositionAndDynamicState();
}
