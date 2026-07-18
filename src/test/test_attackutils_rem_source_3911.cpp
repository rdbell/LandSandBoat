#include "test_attackutils_rem_source_3911.h"
#include "map/utils/attackutils_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "attackutils REM source 3911 failed: " << l << '\n'; return v; } }
auto runAttackutilsRemSource3911SelfTests() -> bool
{
    using namespace attackutilshelpers;
    bool ok = true;
    ok = expect(ShouldUseRangedRemOcc(AttackTypeRanged), "ranged selects ranged REM") && ok;
    ok = expect(ShouldUseRangedRemOcc(AttackTypeRapidShot), "rapid shot selects ranged REM") && ok;
    ok = expect(!ShouldUseRangedRemOcc(AttackTypeNormal), "normal is not ranged REM") && ok;
    ok = expect(ShouldUseMainHandRemOcc(AttackTypeNormal, SlotMain), "normal main selects main REM") && ok;
    ok = expect(!ShouldUseMainHandRemOcc(AttackTypeNormal, SlotMain + 1), "normal non-main skips main REM") && ok;
    ok = expect(!ShouldUseMainHandRemOcc(AttackTypeRanged, SlotMain), "ranged skips main REM") && ok;
    return ok;
}
