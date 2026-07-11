#include "test_camouflage_retain_1391.h"

#include "map/camouflage_retain_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "camouflage retain 1391 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "camouflage retain 1391 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runCamouflageRetain1391SelfTests() -> bool
{
    using namespace camouflageretainhelpers;
    bool ok = true;

    ok = expect(ShouldEvaluateCamouflageRetain(1) && !ShouldEvaluateCamouflageRetain(0), "eval") && ok;
    ok = expect(ShouldStripAllDetectableWithoutRetain(0) && !ShouldStripAllDetectableWithoutRetain(1), "without") && ok;

    ok = expectEq(ResolveCamouflageFacingZone(true, true), CamouflageFacingZone::Behind, "behind first") && ok;
    ok = expectEq(ResolveCamouflageFacingZone(false, true), CamouflageFacingZone::Beside, "beside") && ok;
    ok = expectEq(ResolveCamouflageFacingZone(false, false), CamouflageFacingZone::Front, "front") && ok;

    // Barrage forces 0
    ok = expectEq(ResolveCamouflageRetainChance(true, CamouflageFacingZone::Behind, 100.f, 5.f), static_cast<int16>(0), "barrage") && ok;

    // Behind full: dist > melee+0.6 → 100
    ok = expectEq(ResolveCamouflageRetainChance(false, CamouflageFacingZone::Behind, 6.0f, 5.0f), static_cast<int16>(100), "behind full") && ok;
    // Behind partial: 5.2 > 5.1, < 5.6 → 40 + 1.6*5.2 = 40+8.32 = 48
    ok = expectEq(ResolveCamouflageRetainChance(false, CamouflageFacingZone::Behind, 5.2f, 5.0f), static_cast<int16>(48), "behind partial") && ok;
    // Behind close: dist 5.05 → 0
    ok = expectEq(ResolveCamouflageRetainChance(false, CamouflageFacingZone::Behind, 5.05f, 5.0f), static_cast<int16>(0), "behind close") && ok;

    // Beside full: > melee+5
    ok = expectEq(ResolveCamouflageRetainChance(false, CamouflageFacingZone::Beside, 11.0f, 5.0f), static_cast<int16>(100), "beside full") && ok;
    // Beside partial: 8.5 > 8.3 → 40 + 1.6*8.5 = 40+13.6 = 53
    ok = expectEq(ResolveCamouflageRetainChance(false, CamouflageFacingZone::Beside, 8.5f, 5.0f), static_cast<int16>(53), "beside partial") && ok;

    // Front full: > melee+8.1
    ok = expectEq(ResolveCamouflageRetainChance(false, CamouflageFacingZone::Front, 14.0f, 5.0f), static_cast<int16>(100), "front full") && ok;
    // Front partial: 12.2 > 12.1 → 40 + 1.6*12.2 = 40+19.52 = 59
    ok = expectEq(ResolveCamouflageRetainChance(false, CamouflageFacingZone::Front, 12.2f, 5.0f), static_cast<int16>(59), "front partial") && ok;
    // Front close
    ok = expectEq(ResolveCamouflageRetainChance(false, CamouflageFacingZone::Front, 10.0f, 5.0f), static_cast<int16>(0), "front close") && ok;

    ok = expect(ShouldStripAllDetectableOnFail(40, 41) && !ShouldStripAllDetectableOnFail(40, 40), "strip fail") && ok;
    ok = expect(ShouldStripPartialStealthOnRetain(false) && !ShouldStripPartialStealthOnRetain(true), "partial") && ok;

    ok = expectEq(BaseRetainChance, static_cast<int16>(40), "base pin") && ok;
    ok = expectEq(RotAllowance, static_cast<uint8>(25), "rot pin") && ok;

    return ok;
}
