#include "test_npc_entity_2593.h"

#include <iostream>

#include "entities/npc_entity.h"

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "npc entity self-test failed: " << label << '\n';
    }
    return condition;
}

auto testFlagAccessors() -> bool
{
    auto npc = CNpcEntity{};
    npc.setEntityFlags(0x200);
    npc.hideHP(true);

    bool ok = true;
    ok      = expect(npc.entityFlags() == 0x300, "hide HP sets 0x100 and preserves flags") && ok;
    ok      = expect(!npc.hpHidden(), "hide HP does not set hpHidden") && ok;
    npc.setUntargetable(true);
    ok = expect(npc.GetUntargetable() && npc.hpHidden(), "untargetable and hpHidden share 0x800") && ok;
    npc.setUntargetable(false);
    ok = expect(!npc.GetUntargetable() && !npc.hpHidden(), "untargetable clear") && ok;
    npc.hideHP(false);
    return expect(npc.entityFlags() == 0x200, "hide HP clear preserves unrelated flags") && ok;
}

auto testPrivateStateAccessors() -> bool
{
    auto npc = CNpcEntity{};
    bool ok  = true;
    ok       = expect(npc.widescan() == 1, "widescan defaults to one") && ok;
    ok       = expect(!npc.triggerable(), "triggerable defaults false") && ok;
    ok       = expect(!npc.alwaysRelevant(), "always relevant defaults false") && ok;
    npc.setWidescan(2);
    npc.setTriggerable(true);
    npc.setAlwaysRelevant(true);
    ok = expect(npc.widescan() == 2, "widescan setter") && ok;
    ok = expect(npc.triggerable(), "triggerable setter") && ok;
    ok = expect(npc.alwaysRelevant(), "always relevant setter") && ok;
    return ok;
}

} // namespace

auto runNpcEntity2593SelfTests() -> bool
{
    return testFlagAccessors() && testPrivateStateAccessors();
}
