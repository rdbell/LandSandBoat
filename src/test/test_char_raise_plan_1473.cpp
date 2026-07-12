#include "test_char_raise_plan_1473.h"

#include "map/char_raise_plan_capacity.h"

#include <chrono>
#include <iostream>

auto runCharRaisePlan1473SelfTests() -> bool
{
    using namespace std::chrono_literals;

    const auto none = charraiseplanhelpers::Build({});
    bool ok = !none.handled;

    const auto raise1 = charraiseplanhelpers::Build({ .hasRaise = 1, .maxHP = 1000, .mainLevel = 75 });
    ok = raise1.handled && raise1.weaknessLevel == 1 && raise1.applyWeakness && raise1.weaknessDuration == 5min &&
         raise1.animation == ActionAnimation::Raise && raise1.hpReturned == 100 && raise1.expReturnRatio == 0.5 && ok;

    const auto mijin = charraiseplanhelpers::Build({ .hasRaise = 3, .mijinGakure = true, .mijinReraise = true, .maxHP = 1000 });
    ok = !mijin.applyWeakness && mijin.animation == ActionAnimation::Raise && mijin.hpReturned == 1000 && mijin.expReturnRatio == 0.0 && ok;

    const auto raise2 = charraiseplanhelpers::Build({ .hasRaise = 2, .maxHP = 1000, .mainLevel = 51 });
    ok = raise2.animation == ActionAnimation::Raise2 && raise2.hpReturned == 250 && raise2.expReturnRatio == 0.75 && ok;

    const auto raise3 = charraiseplanhelpers::Build({ .hasRaise = 3, .weaknessLevel = 2, .maxHP = 1000, .mainLevel = 75 });
    ok = raise3.weaknessLevel == 2 && raise3.animation == ActionAnimation::Raise3 && raise3.hpReturned == 500 && raise3.expReturnRatio == 0.9 && ok;

    const auto arise = charraiseplanhelpers::Build({ .hasRaise = 4, .hasArise = true, .maxHP = 1000, .mainLevel = 75 });
    ok = arise.weaknessDuration == 3min && arise.animation == ActionAnimation::Arise && arise.hpReturned == 1000 &&
         arise.expReturnRatio == 0.9 && arise.applyReraise3 && ok;

    const auto unknown = charraiseplanhelpers::Build({ .hasRaise = 5, .maxHP = 1000 });
    ok = unknown.handled && unknown.hpReturned == 1 && unknown.animation == ActionAnimation{} && ok;

    if (!ok)
    {
        std::cerr << "char raise plan 1473 self-test failed\n";
    }
    return ok;
}
