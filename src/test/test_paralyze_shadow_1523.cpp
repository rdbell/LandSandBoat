#include "test_paralyze_shadow_1523.h"

#include "map/paralyze_shadow_capacity.h"

#include <iostream>

namespace
{
using paralyzeshadowhelpers::AbsorbByShadowResult;
using paralyzeshadowhelpers::EffectCopyImage;
using paralyzeshadowhelpers::EffectCopyImage2;
using paralyzeshadowhelpers::EffectCopyImage3;
using paralyzeshadowhelpers::GetHitRateEx;
using paralyzeshadowhelpers::IsAbsorbByShadow;
using paralyzeshadowhelpers::IsParalyzed;
using paralyzeshadowhelpers::ShadowModKind;

auto Check() -> bool
{
    if (IsParalyzed(0, 0) || !IsParalyzed(100, 0) || !IsParalyzed(100, 99))
    {
        return false;
    }
    if (!IsParalyzed(25, 24) || IsParalyzed(25, 25) || IsParalyzed(-1, 0))
    {
        return false;
    }

    {
        const auto r = IsAbsorbByShadow(1, 0, 0, false, false, false);
        if (!r.absorbed || r.remaining != 0 || r.usedMod != ShadowModKind::Utsusemi || !r.delCopyImage)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(4, 0, 0, true, true, true);
        if (!r.absorbed || r.remaining != 3 || !r.setIcon || r.icon != EffectCopyImage3 || !r.applyCEEnmity)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(3, 0, 0, true, true, false);
        if (r.icon != EffectCopyImage2 || r.applyCEEnmity)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(2, 0, 0, true, true, true);
        if (r.icon != EffectCopyImage || !r.applyCEEnmity)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(5, 0, 0, true, true, true);
        if (r.remaining != 4 || r.setIcon || r.applyCEEnmity)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(2, 0, 0, false, true, true);
        if (r.setIcon || r.applyCEEnmity)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(0, 2, 19, false, false, false);
        if (r.absorbed)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(0, 2, 20, false, false, false);
        if (!r.absorbed || r.remaining != 1 || r.usedMod != ShadowModKind::Blink || r.delBlink)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(0, 1, 50, false, false, false);
        if (!r.absorbed || r.remaining != 0 || !r.delBlink || r.delCopyImage)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(1, 5, 0, false, false, false);
        if (!r.absorbed || r.usedMod != ShadowModKind::Utsusemi || !r.delCopyImage)
        {
            return false;
        }
    }
    {
        const auto r = IsAbsorbByShadow(0, 0, 50, false, false, false);
        if (r.absorbed)
        {
            return false;
        }
    }

    if (GetHitRateEx(true, false, 0.5) != 100 || GetHitRateEx(false, true, 0.5) != 100)
    {
        return false;
    }
    if (GetHitRateEx(false, false, 0.75) != 75 || GetHitRateEx(false, false, 0.999) != 99)
    {
        return false;
    }
    if (GetHitRateEx(false, false, 0.0) != 0)
    {
        return false;
    }

    return true;
}
} // namespace

auto runParalyzeShadow1523SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "paralyze_shadow_1523 self-tests failed\n";
        return false;
    }
    return true;
}
