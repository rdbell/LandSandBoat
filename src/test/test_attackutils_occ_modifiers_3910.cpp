#include "test_attackutils_occ_modifiers_3910.h"
#include "map/utils/attackutils_capacity.h"
#include <iostream>
namespace { auto expect(bool v, const char* l) -> bool { if (!v) std::cerr << "attackutils Occ modifiers 3910 failed: " << l << '\n'; return v; } }
auto runAttackutilsOccModifiers3910SelfTests() -> bool
{
    using namespace attackutilshelpers;
    bool ok = true;
    ok = expect(OccExtraDmgMultiplier(250) == 2.5f, "extra multiplier scale") && ok;
    ok = expect(OccExtraDmgMultiplier(-50) == -0.5f, "negative multiplier scale") && ok;
    ok = expect(OccExtraDmgChance(59) == 5 && OccExtraDmgChance(-59) == -5, "extra chance truncates toward zero") && ok;
    ok = expect(RemOccChance(19) == 1 && RemOccChance(-19) == -1, "rem chance truncates toward zero") && ok;
    return ok;
}
