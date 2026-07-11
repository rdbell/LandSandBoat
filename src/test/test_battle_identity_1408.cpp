#include "test_battle_identity_1408.h"

#include "map/battle_identity_capacity.h"

#include <iostream>
#include <limits>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle identity 1408 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleIdentity1408SelfTests() -> bool
{
    battleidentityhelpers::State state;
    bool ok = expect(state.GetBattleID() == 0, "battle id defaults to zero");
    ok      = expect(state.GetBattleTargetID() == 0, "target id defaults to zero") && ok;

    state.SetBattleID(0x1234);
    state.SetBattleTargetID(0x5678);
    ok = expect(state.GetBattleID() == 0x1234, "battle id round trip") && ok;
    ok = expect(state.GetBattleTargetID() == 0x5678, "target id round trip") && ok;

    state.SetBattleID(std::numeric_limits<uint16>::max());
    state.SetBattleTargetID(std::numeric_limits<uint16>::max());
    ok = expect(state.GetBattleID() == std::numeric_limits<uint16>::max(), "battle id full range") && ok;
    ok = expect(state.GetBattleTargetID() == std::numeric_limits<uint16>::max(), "target id full range") && ok;

    state.SetBattleID(0);
    state.SetBattleTargetID(0);
    ok = expect(state.GetBattleID() == 0 && state.GetBattleTargetID() == 0, "clear independently") && ok;
    return ok;
}
