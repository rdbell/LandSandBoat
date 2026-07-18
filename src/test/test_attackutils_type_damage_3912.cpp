#include "test_attackutils_type_damage_3912.h"
#include "map/utils/attackutils_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "attackutils type damage 3912 failed: " << l << '\n'; return v; } }
auto runAttackutilsTypeDamage3912SelfTests() -> bool
{
    using attackutilshelpers::ApplyTypeDoubleDamage;
    bool ok = true;
    ok = expect(ApplyTypeDoubleDamage(50, 2) == 100, "double") && ok;
    ok = expect(ApplyTypeDoubleDamage(50, 3) == 150, "triple") && ok;
    ok = expect(ApplyTypeDoubleDamage(0, 9) == 0, "zero") && ok;
    ok = expect(ApplyTypeDoubleDamage(0xFFFFFFFFu, 2) == 0xFFFFFFFEu, "uint32 multiplication") && ok;
    return ok;
}
