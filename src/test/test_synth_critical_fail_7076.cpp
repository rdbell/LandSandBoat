#include "test_synth_critical_fail_7076.h"

#include "map/synth_critical_fail.h"

#include <iostream>

auto runSynthCriticalFail7076SelfTests() -> bool
{
    constexpr std::array ingredients{ uint16{ 10 }, uint16{ 0 }, uint16{ 30 }, uint16{ 0 }, uint16{ 0 }, uint16{ 0 }, uint16{ 0 }, uint16{ 0 } };
    const auto           missing = synthcriticalfailhelpers::MakePlan(false, ingredients, 1);
    const auto           normal  = synthcriticalfailhelpers::MakePlan(true, ingredients, 1);
    const auto           zero    = synthcriticalfailhelpers::MakePlan(true, ingredients, 0);
    const auto           test    = synthcriticalfailhelpers::MakePlan(true, ingredients, synthcriticalfailhelpers::ExcludedZone);
    const auto           high    = synthcriticalfailhelpers::MakePlan(true, ingredients, synthcriticalfailhelpers::MaxZoneID);
    const bool           ok      = missing == synthcriticalfailhelpers::Plan{} &&
                                   normal.markBroken[0] && !normal.markBroken[1] && normal.markBroken[2] &&
                                   normal.sendInterruptedInfo && normal.sendInterruptedAnswer && normal.commitTransaction && normal.removeTransaction &&
                                   normal.setAnimationNone && normal.setUpdateHP && normal.sendCharStatus &&
                                   !zero.sendInterruptedInfo && !test.sendInterruptedInfo && !high.sendInterruptedInfo;
    if (!ok)
    {
        std::cerr << "synth critical fail 7076 self-test failed\n";
    }
    return ok;
}
