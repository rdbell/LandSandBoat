#include "test_death_type_1685.h"

#include "map/death_type_capacity.h"

#include <iostream>

namespace
{
using namespace deathtypehelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "death type 1685 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runDeathType1685SelfTests() -> bool
{
    bool ok = true;

    // --- DEATH_TYPE ordinal pins ---
    ok = expect(DeathTypeNone == 0, "DeathTypeNone pin") && ok;
    ok = expect(Physical == 1, "Physical pin") && ok;
    ok = expect(Magical == 2, "Magical pin") && ok;
    ok = expect(WSPhysical == 3, "WSPhysical pin") && ok;
    ok = expect(WSMagical == 4, "WSMagical pin") && ok;
    ok = expect(MaxKnownDeathType == 4, "MaxKnownDeathType pin") && ok;

    // --- SetDeathType store-as-is (any uint8) ---
    ok = expect(SetDeathType(0) == 0, "set 0") && ok;
    ok = expect(SetDeathType(1) == 1, "set 1") && ok;
    ok = expect(SetDeathType(2) == 2, "set 2") && ok;
    ok = expect(SetDeathType(3) == 3, "set 3") && ok;
    ok = expect(SetDeathType(4) == 4, "set 4") && ok;
    ok = expect(SetDeathType(5) == 5, "set 5 unknown") && ok;
    ok = expect(SetDeathType(99) == 99, "set 99") && ok;
    ok = expect(SetDeathType(255) == 255, "set 255") && ok;

    // --- GetDeathType load-as-is ---
    ok = expect(GetDeathType(0) == 0, "get 0") && ok;
    ok = expect(GetDeathType(4) == 4, "get 4") && ok;
    ok = expect(GetDeathType(5) == 5, "get 5") && ok;
    ok = expect(GetDeathType(255) == 255, "get 255") && ok;

    // --- round-trip via named constants ---
    ok = expect(GetDeathType(SetDeathType(DeathTypeNone)) == DeathTypeNone, "round-trip NONE") && ok;
    ok = expect(GetDeathType(SetDeathType(Physical)) == Physical, "round-trip PHYSICAL") && ok;
    ok = expect(GetDeathType(SetDeathType(Magical)) == Magical, "round-trip MAGICAL") && ok;
    ok = expect(GetDeathType(SetDeathType(WSPhysical)) == WSPhysical, "round-trip WS_PHYSICAL") && ok;
    ok = expect(GetDeathType(SetDeathType(WSMagical)) == WSMagical, "round-trip WS_MAGICAL") && ok;
    ok = expect(GetDeathType(SetDeathType(255)) == 255, "round-trip 255") && ok;

    // --- IsKnownDeathType catalog 0..4 ---
    ok = expect(IsKnownDeathType(DeathTypeNone), "known NONE") && ok;
    ok = expect(IsKnownDeathType(Physical), "known PHYSICAL") && ok;
    ok = expect(IsKnownDeathType(Magical), "known MAGICAL") && ok;
    ok = expect(IsKnownDeathType(WSPhysical), "known WS_PHYSICAL") && ok;
    ok = expect(IsKnownDeathType(WSMagical), "known WS_MAGICAL") && ok;
    ok = expect(IsKnownDeathType(0), "known 0") && ok;
    ok = expect(IsKnownDeathType(4), "known 4") && ok;
    ok = expect(!IsKnownDeathType(5), "unknown 5") && ok;
    ok = expect(!IsKnownDeathType(6), "unknown 6") && ok;
    ok = expect(!IsKnownDeathType(99), "unknown 99") && ok;
    ok = expect(!IsKnownDeathType(255), "unknown 255") && ok;

    // --- Set does not require known ---
    ok = expect(!IsKnownDeathType(5) && SetDeathType(5) == 5, "set unknown as-is") && ok;

    return ok;
}
