#include "test_attack_post_swing_1399.h"

#include "map/attack_post_swing_capacity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attack post swing 1399 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runAttackPostSwing1399SelfTests() -> bool
{
    using namespace attackpostswinghelpers;
    bool ok = true;

    ok = expect(NormalizeMissParam(ResolutionMiss, 15, 99) == 0, "ordinary miss zero") && ok;
    ok = expect(NormalizeMissParam(ResolutionMiss, MsgShadowAbsorb, 1) == 1, "shadow preserve") && ok;
    ok = expect(NormalizeMissParam(0, 15, 99) == 99, "hit preserve") && ok;

    ok = expect(ShouldRunEnspellAndSpikes(0, 0), "hit effects") && ok;
    ok = expect(ShouldRunEnspellAndSpikes(2, 0) && ShouldRunEnspellAndSpikes(4, 0), "guard block effects") && ok;
    ok = expect(!ShouldRunEnspellAndSpikes(ResolutionMiss, 0) && !ShouldRunEnspellAndSpikes(ResolutionParry, 0), "miss parry block") && ok;
    ok = expect(!ShouldRunEnspellAndSpikes(0, AttackTypeDaken), "Daken block") && ok;
    ok = expect(ShouldRunEnspell(1) && !ShouldRunEnspell(0), "live enspell target") && ok;

    int battutaChecks = 0;
    auto battuta = [&]() { ++battutaChecks; return true; };
    ok = expect(!ShouldRunParrySpikes(0, battuta) && battutaChecks == 0, "Battuta lazy non-parry") && ok;
    ok = expect(ShouldRunParrySpikes(ResolutionParry, battuta) && battutaChecks == 1, "Battuta parry") && ok;

    ok = expect(ResolutionMiss == 1 && ResolutionParry == 3 && AttackTypeDaken == 9 && MsgShadowAbsorb == 31, "pins") && ok;
    return ok;
}
