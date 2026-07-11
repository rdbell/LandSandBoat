#include "test_status_effect_save_1373.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect save 1373 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "status effect save 1373 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runStatusEffectSave1373SelfTests() -> bool
{
    using namespace statuseffecthelpers;
    bool ok = true;

    ok = expect(ShouldRejectNonPCSave(false) && !ShouldRejectNonPCSave(true), "non-pc") && ok;
    ok = expect(ShouldStripOnSave(true, true, false), "logout strip") && ok;
    ok = expect(ShouldStripOnSave(false, false, true), "zone strip") && ok;
    ok = expect(!ShouldStripOnSave(true, false, true), "logout no flag") && ok;
    ok = expect(!ShouldStripOnSave(false, true, false), "zone no flag") && ok;
    ok = expect(ShouldSkipDeletedOnSave(true) && !ShouldSkipDeletedOnSave(false), "deleted") && ok;
    ok = expect(ShouldPersistEffect(10, 30) && ShouldPersistEffect(0, 0), "persist") && ok;
    ok = expect(!ShouldPersistEffect(0, 30), "no persist expired") && ok;

    ok = expect(IsCopyImageEffect(StatusIDCopyImage) && IsBlinkEffect(StatusIDBlink), "ids") && ok;
    ok = expect(IsStoneskinEffect(StatusIDStoneskin), "stoneskin") && ok;
    ok = expect(ShouldResyncUtsusemiPower(StatusIDCopyImage) && ShouldResyncBlinkPower(StatusIDBlink), "resync") && ok;
    ok = expect(ShouldResyncStoneskinPower(StatusIDStoneskin), "resync skin") && ok;

    ok = expectEq(ComputePersistedDurationSeconds(0, 0, false), static_cast<uint32>(0), "perm") && ok;
    ok = expectEq(ComputePersistedDurationSeconds(100, 40, true), static_cast<uint32>(100), "offline tick") && ok;
    ok = expectEq(ComputePersistedDurationSeconds(100, 40, false), static_cast<uint32>(40), "real rem") && ok;
    ok = expectEq(ComputePersistedDurationSeconds(100, 0, false), static_cast<uint32>(0), "expired rem") && ok;
    ok = expectEq(RealDurationSeconds(200, 150), static_cast<int64>(50), "real dur") && ok;
    ok = expect(ShouldLoadCopyImageUtsusemi(StatusIDCopyImage) && ShouldLoadBlinkMod(StatusIDBlink), "load") && ok;

    return ok;
}
