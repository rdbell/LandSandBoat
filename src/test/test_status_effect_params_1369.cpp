#include "test_status_effect_params_1369.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect params 1369 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runStatusEffectParams1369SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(ShouldRejectEffectIDOutOfRange(MaxEffectID, MaxEffectID), "max id") && ok;
    ok = expect(!ShouldRejectEffectIDOutOfRange(0, MaxEffectID), "ok id") && ok;
    ok = expect(ShouldRejectNoneZeroSub(StatusIDNoneEffect, 0, StatusIDNoneEffect), "none zero") && ok;
    ok = expect(!ShouldRejectNoneZeroSub(StatusIDNoneEffect, 1, StatusIDNoneEffect), "none sub") && ok;
    ok = expect(HasEffectSource(SourceTypeEquippedItem, 5) && !HasEffectSource(SourceTypeNone, 5), "source") && ok;
    ok = expect(IsEquippedItemSource(SourceTypeEquippedItem) && IsFoodSource(SourceTypeFood), "source kinds") && ok;
    ok = expect(ShouldSetItemScriptName(true, true) && !ShouldSetItemScriptName(true, false), "script pair") && ok;
    ok = expect(FormatItemScriptName("foo") == "items/foo", "item name") && ok;
    ok = expect(FormatEffectsScriptName("bar") == "effects/bar", "effects name") && ok;

    // Default effects path
    ok = expect(ShouldUseEffectsScriptPath(false, false, false, false, false, 0), "default effects") && ok;
    ok = expect(ShouldUseEffectsScriptPath(false, false, false, false, true, 0), "food fov") && ok;
    ok = expect(!ShouldUseEffectsScriptPath(false, false, false, false, true, 9), "food item") && ok;
    ok = expect(!ShouldUseEffectsScriptPath(true, false, false, false, false, 0), "enchant item") && ok;
    ok = expect(!ShouldUseEffectsScriptPath(false, false, true, false, false, 0), "enchantment id") && ok;
    ok = expect(!ShouldUseEffectsScriptPath(false, false, false, true, false, 0), "equipped source") && ok;

    ok = expect(ShouldUseItemSubTypeScript(false, 10, true) && !ShouldUseItemSubTypeScript(true, 10, true), "sub item") && ok;
    ok = expect(!ShouldUseItemSubTypeScript(false, 0, true), "sub zero") && ok;

    return ok;
}
