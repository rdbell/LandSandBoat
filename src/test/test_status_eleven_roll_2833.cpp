#include "test_status_eleven_roll_2833.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status eleven roll 2833 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusElevenRoll2833SelfTests() -> bool
{
    using statuseffecthelpers::ElevenRollIDFirst;
    using statuseffecthelpers::ElevenRollIDLast;
    using statuseffecthelpers::ElevenRollSubPower;
    using statuseffecthelpers::HasElevenRollInSet;
    using statuseffecthelpers::IsElevenRollEffect;
    using statuseffecthelpers::PlanCheckForElevenRoll;
    using statuseffecthelpers::RuneistsRollID;

    bool ok = true;

    // 1) empty / unequal length
    ok = expect(!HasElevenRollInSet({}, {}), "empty") && ok;
    {
        const uint16 ids[] = { ElevenRollIDFirst };
        ok                 = expect(!HasElevenRollInSet(ids, {}), "ids only") && ok;
        const uint16 sps[] = { ElevenRollSubPower };
        ok                 = expect(!HasElevenRollInSet({}, sps), "sps only") && ok;
    }

    // 2) single-pair gates via set scan == IsElevenRollEffect
    const struct
    {
        uint16      statusID;
        uint16      subPower;
        bool        want;
        const char* label;
    } singles[] = {
        { ElevenRollIDFirst, ElevenRollSubPower, true, "fighters 11" },
        { ElevenRollIDLast, ElevenRollSubPower, true, "naturalists 11" },
        { RuneistsRollID, ElevenRollSubPower, true, "runeists 11" },
        { ElevenRollIDFirst, 10, false, "fighters 10" },
        { 400, ElevenRollSubPower, false, "not roll @11" },
        { 309, ElevenRollSubPower, false, "bust @11" },
        { 310, 11, true, "numeric fighters" },
        { 339, 11, true, "numeric naturalists" },
        { 600, 11, true, "numeric runeists" },
        { 340, 11, false, "after naturalists" },
        { 599, 11, false, "near runeists" },
        { 601, 11, false, "after runeists" },
    };
    for (const auto& c : singles)
    {
        const uint16 ids[] = { c.statusID };
        const uint16 sps[] = { c.subPower };
        const bool   got   = HasElevenRollInSet(ids, sps);
        const bool   direct =
            IsElevenRollEffect(c.statusID, c.subPower, ElevenRollIDFirst, ElevenRollIDLast, RuneistsRollID);
        ok = expect(got == c.want && got == direct, c.label) && ok;
        ok = expect(PlanCheckForElevenRoll(ids, sps) == got, "plan alias") && ok;
    }

    // 3) multi-pair any-hit
    {
        const uint16 missIDs[] = { ElevenRollIDFirst, ElevenRollIDLast };
        const uint16 missSPs[] = { 10, 5 };
        ok                     = expect(!HasElevenRollInSet(missIDs, missSPs), "multi miss") && ok;

        const uint16 hitIDs[] = { ElevenRollIDFirst, ElevenRollIDLast, 400 };
        const uint16 hitSPs[] = { 10, ElevenRollSubPower, ElevenRollSubPower };
        ok                    = expect(HasElevenRollInSet(hitIDs, hitSPs), "multi mid hit") && ok;

        const uint16 runeIDs[] = { 400, RuneistsRollID };
        const uint16 runeSPs[] = { ElevenRollSubPower, ElevenRollSubPower };
        ok                     = expect(HasElevenRollInSet(runeIDs, runeSPs), "multi runeists") && ok;
    }

    // 4) min-length truncation
    {
        const uint16 ids[] = { ElevenRollIDFirst, 400 };
        const uint16 sps[] = { ElevenRollSubPower };
        ok                 = expect(HasElevenRollInSet(ids, sps), "extra id ignored") && ok;

        const uint16 shortIDs[] = { 400 };
        const uint16 longSPs[]  = { ElevenRollSubPower, ElevenRollSubPower };
        ok                      = expect(!HasElevenRollInSet(shortIDs, longSPs), "extra sp ignored") && ok;
    }

    // 5) constants pins
    ok = expect(ElevenRollIDFirst == 310, "first 310") && ok;
    ok = expect(ElevenRollIDLast == 339, "last 339") && ok;
    ok = expect(RuneistsRollID == 600, "runeists 600") && ok;
    ok = expect(ElevenRollSubPower == 11, "subPower 11") && ok;

    // 6) PlanCheckForElevenRoll empty
    ok = expect(!PlanCheckForElevenRoll({}, {}), "plan empty") && ok;

    return ok;
}
