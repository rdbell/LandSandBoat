#include "test_latent_condition_eval_1360.h"

#include "map/latent_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "latent condition eval 1360 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLatentConditionEval1360SelfTests() -> bool
{
    bool ok = true;

    // Zone / synth / song / eleven
    ok = expect(latenthelpers::EvaluateZone(100, 100), "zone") && ok;
    ok = expect(!latenthelpers::EvaluateZone(100, 101), "zone miss") && ok;
    ok = expect(latenthelpers::EvaluateSynthTrainee(390, false), "synth under 40") && ok; // 390/10=39
    ok = expect(!latenthelpers::EvaluateSynthTrainee(400, false), "synth 40") && ok;
    ok = expect(!latenthelpers::EvaluateSynthTrainee(100, true), "synth imagery") && ok;
    ok = expect(latenthelpers::EvaluateSongRollActive(true), "song roll") && ok;
    ok = expect(latenthelpers::EvaluateElevenRollActive(true), "eleven") && ok;
    ok = expect(!latenthelpers::EvaluateElevenRollActive(false), "no eleven") && ok;

    // Time of day
    ok = expect(latenthelpers::EvaluateTimeOfDay(12, 0), "daytime") && ok;
    ok = expect(!latenthelpers::EvaluateTimeOfDay(5, 0), "not daytime") && ok;
    ok = expect(latenthelpers::EvaluateTimeOfDay(20, 1), "nighttime") && ok;
    ok = expect(latenthelpers::EvaluateTimeOfDay(3, 1), "night early") && ok;
    ok = expect(latenthelpers::EvaluateTimeOfDay(18, 2), "dusk dawn") && ok;
    ok = expect(!latenthelpers::EvaluateTimeOfDay(12, 2), "not dusk dawn") && ok;

    // Hour of day
    ok = expect(latenthelpers::EvaluateHourOfDay(4, 1), "new day") && ok;
    ok = expect(latenthelpers::EvaluateHourOfDay(6, 2), "dawn") && ok;
    ok = expect(latenthelpers::EvaluateHourOfDay(10, 3), "day hours") && ok;
    ok = expect(latenthelpers::EvaluateHourOfDay(17, 4), "dusk hours") && ok;
    ok = expect(latenthelpers::EvaluateHourOfDay(19, 5), "evening") && ok;
    ok = expect(latenthelpers::EvaluateHourOfDay(22, 6), "dead night") && ok;
    ok = expect(latenthelpers::EvaluateHourOfDay(2, 6), "dead night early") && ok;
    ok = expect(!latenthelpers::EvaluateHourOfDay(10, 1), "not new day") && ok;

    // Weekday / moon
    ok = expect(latenthelpers::EvaluateWeekdayMatch(3, 3), "weekday") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(3, 0, 0), "new moon low") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(8, 1, 0), "new moon waning") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(20, 2, 1), "waxing crescent") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(50, 2, 2), "first quarter") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(70, 2, 3), "waxing gibbous") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(96, 0, 4), "full moon") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(92, 2, 4), "full waxing") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(80, 1, 5), "waning gibbous") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(50, 1, 6), "last quarter") && ok;
    ok = expect(latenthelpers::EvaluateMoonPhase(20, 1, 7), "waning crescent") && ok;
    ok = expect(!latenthelpers::EvaluateMoonPhase(50, 2, 0), "not new") && ok;

    // Job multiple
    ok = expect(latenthelpers::EvaluateJobMultiple(5, 0), "odd") && ok;
    ok = expect(!latenthelpers::EvaluateJobMultiple(4, 0), "even") && ok;
    ok = expect(latenthelpers::EvaluateJobMultiple(9, 3), "div 3") && ok;
    ok = expect(!latenthelpers::EvaluateJobMultiple(10, 3), "not div 3") && ok;
    ok = expect(latenthelpers::EvaluateJobMultipleAtNight(5, 0, true), "odd night") && ok;
    ok = expect(!latenthelpers::EvaluateJobMultipleAtNight(5, 0, false), "odd day") && ok;

    // Weapon absolute / broken / flags
    ok = expect(latenthelpers::EvaluateWeaponDrawnHpAbsolute(true, 49, 50), "drawn hp abs") && ok;
    ok = expect(!latenthelpers::EvaluateWeaponDrawnHpAbsolute(true, 50, 50), "drawn hp abs eq") && ok;
    ok = expect(latenthelpers::EvaluateWeaponBroken(true, true, true), "broken") && ok;
    ok = expect(!latenthelpers::EvaluateWeaponBroken(false, true, true), "not weapon slot") && ok;
    ok = expect(latenthelpers::EvaluateInFlag(true), "flag") && ok;
    ok = expect(latenthelpers::EvaluateInGarrison(true, 50, 50), "garrison") && ok;
    ok = expect(!latenthelpers::EvaluateInGarrison(true, 49, 50), "garrison low") && ok;

    // Nation / weather / vs
    ok = expect(latenthelpers::EvaluateNationCitizen(1, 1), "citizen") && ok;
    ok = expect(latenthelpers::EvaluateNationControlUnder(true, true, true), "control under") && ok;
    ok = expect(latenthelpers::EvaluateNationControlOutside(true, true, true), "control out") && ok;
    ok = expect(latenthelpers::EvaluateZoneHomeNation(0, 0, true), "home nation") && ok;
    ok = expect(latenthelpers::EvaluateWeatherMatch(5, 5), "weather") && ok;
    ok = expect(latenthelpers::EvaluateWeatherElementMatch(3, 3), "element") && ok;
    ok = expect(latenthelpers::EvaluateVsTargetValue(true, 7, 7), "vs target") && ok;
    ok = expect(!latenthelpers::EvaluateVsTargetValue(false, 7, 7), "vs no target") && ok;

    // Avatar / job in party helpers
    ok = expect(latenthelpers::IsLiveAvatar(false, 5), "live avatar") && ok;
    ok = expect(!latenthelpers::IsLiveAvatar(true, 5), "dead avatar") && ok;
    ok = expect(!latenthelpers::IsLiveAvatar(false, 21), "not avatar id") && ok;
    ok = expect(latenthelpers::EvaluateJobInPartyMember(true, true), "job member") && ok;
    ok = expect(!latenthelpers::EvaluateJobInPartyMember(false, true), "self skip") && ok;

    return ok;
}
