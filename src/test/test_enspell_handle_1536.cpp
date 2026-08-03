#include "test_enspell_handle_1536.h"

#include "map/enspell_handle_capacity.h"

#include <iostream>

namespace
{
using enspellhandlehelpers::AspirSambaAmount;
using enspellhandlehelpers::AspirSambaRollUpper;
using enspellhandlehelpers::BloodWeaponAbsorbed;
using enspellhandlehelpers::ClassifyDazeClearPath;
using enspellhandlehelpers::ClassifyEnspellPath;
using enspellhandlehelpers::DazeClearPath;
using enspellhandlehelpers::DazePowerFromMembership;
using enspellhandlehelpers::DrainSambaAmount;
using enspellhandlehelpers::DrainSambaRollUpper;
using enspellhandlehelpers::ElementalEnspellSubEffect;
using enspellhandlehelpers::ElementalEnspellTierAndElement;
using enspellhandlehelpers::EnspellAuspice;
using enspellhandlehelpers::EnspellBloodWeapon;
using enspellhandlehelpers::EnspellIFire;
using enspellhandlehelpers::EnspellILight;
using enspellhandlehelpers::EnspellIIFire;
using enspellhandlehelpers::EnspellPath;
using enspellhandlehelpers::EnspellShouldProc;
using enspellhandlehelpers::GripAddEffectGate;
using enspellhandlehelpers::MobPetAddEffectGate;
using enspellhandlehelpers::MsgAddEffectAdditionalDamage;
using enspellhandlehelpers::MsgAddEffectDamage;
using enspellhandlehelpers::MsgAddEffectRecoversHP;
using enspellhandlehelpers::NormalizeEnspellDamageParam;
using enspellhandlehelpers::RuneUsesNewest;
using enspellhandlehelpers::SambaDaze;
using enspellhandlehelpers::SelectPreviousDaze;
using enspellhandlehelpers::ShouldApplyAspirSamba;
using enspellhandlehelpers::ShouldApplyDrainSamba;
using enspellhandlehelpers::ShouldApplyHasteSamba;
using enspellhandlehelpers::ShouldRewriteAddEffectToRecoverHP;
using enspellhandlehelpers::WeaponAddEffectPriority;

auto Check() -> bool
{
    if (SelectPreviousDaze(true, true, true, false) != SambaDaze::Drain)
    {
        return false;
    }
    if (SelectPreviousDaze(true, true, true, true) != SambaDaze::Aspir)
    {
        return false;
    }
    if (SelectPreviousDaze(false, false, true, false) != SambaDaze::Haste)
    {
        return false;
    }
    if (SelectPreviousDaze(false, false, false, false) != SambaDaze::None)
    {
        return false;
    }

    if (ClassifyDazeClearPath(true, true, false, false) != DazeClearPath::Party)
    {
        return false;
    }
    if (ClassifyDazeClearPath(false, false, true, true) != DazeClearPath::TrustMasterParty)
    {
        return false;
    }
    if (ClassifyDazeClearPath(true, false, false, false) != DazeClearPath::AttackerOnly)
    {
        return false;
    }

    if (!EnspellShouldProc(0, 0, 50, 1) || !EnspellShouldProc(5, 0, 99, 0))
    {
        return false;
    }
    if (!EnspellShouldProc(5, 50, 49, 0) || EnspellShouldProc(5, 50, 50, 0))
    {
        return false;
    }
    if (EnspellShouldProc(0, 0, 0, 0))
    {
        return false;
    }

    if (ClassifyEnspellPath(EnspellBloodWeapon, false, false, true) != EnspellPath::BloodWeapon)
    {
        return false;
    }
    if (ClassifyEnspellPath(EnspellBloodWeapon, true, false, true) != EnspellPath::None)
    {
        return false;
    }
    if (ClassifyEnspellPath(1, false, true, true) != EnspellPath::Rune)
    {
        return false;
    }
    if (ClassifyEnspellPath(EnspellAuspice, false, false, true) != EnspellPath::Auspice)
    {
        return false;
    }
    if (ClassifyEnspellPath(EnspellAuspice, false, false, false) != EnspellPath::None)
    {
        return false;
    }
    if (ClassifyEnspellPath(EnspellIIFire, false, false, true) != EnspellPath::ElementalII)
    {
        return false;
    }
    if (ClassifyEnspellPath(EnspellIIFire, false, false, false) != EnspellPath::ElementalIISkip)
    {
        return false;
    }
    if (ClassifyEnspellPath(EnspellIFire, false, false, false) != EnspellPath::ElementalI)
    {
        return false;
    }

    {
        std::uint8_t tier = 0;
        std::uint8_t el   = 0;
        ElementalEnspellTierAndElement(EnspellIFire, EnspellPath::ElementalI, tier, el);
        if (tier != 1 || el != 1)
        {
            return false;
        }
        ElementalEnspellTierAndElement(EnspellILight, EnspellPath::ElementalI, tier, el);
        if (tier != 3 || el != 7)
        {
            return false;
        }
        ElementalEnspellTierAndElement(EnspellIIFire, EnspellPath::ElementalII, tier, el);
        if (tier != 2 || el != 1)
        {
            return false;
        }
        ElementalEnspellTierAndElement(EnspellAuspice, EnspellPath::Auspice, tier, el);
        if (tier != 2 || el != 7)
        {
            return false;
        }
    }

    if (ElementalEnspellSubEffect(EnspellIFire, false) != 1)
    {
        return false;
    }
    if (ElementalEnspellSubEffect(10 /* II Ice */, true) != 2)
    {
        return false;
    }
    if (!RuneUsesNewest(1) || RuneUsesNewest(2))
    {
        return false;
    }

    if (BloodWeaponAbsorbed(100, true, 5) != 110 || BloodWeaponAbsorbed(100, false, 5) != 100)
    {
        return false;
    }
    if (BloodWeaponAbsorbed(5, true, 10) != 5)
    {
        return false;
    }
    if (!ShouldRewriteAddEffectToRecoverHP(MsgAddEffectDamage, -5))
    {
        return false;
    }
    {
        std::int32_t  p   = 0;
        std::uint16_t msg = 0;
        NormalizeEnspellDamageParam(-20, p, msg);
        if (p != 20 || msg != MsgAddEffectRecoversHP)
        {
            return false;
        }
        NormalizeEnspellDamageParam(15, p, msg);
        if (p != 15 || msg != MsgAddEffectAdditionalDamage)
        {
            return false;
        }
    }

    if (DrainSambaRollUpper(1, 100) != 4)
    {
        return false;
    }
    if (DrainSambaAmount(1, 100, 3, 0, 20) != 3)
    {
        return false;
    }
    if (DrainSambaAmount(1, 100, 3, 0, 2) != 0)
    {
        return false;
    }
    if (DrainSambaAmount(1, 100, 50, 0, 20) != 10)
    {
        return false;
    }
    if (DrainSambaAmount(1, 100, 10, 10, 100) != 6)
    {
        return false;
    }
    if (AspirSambaRollUpper(1, 100) != 2)
    {
        return false;
    }
    if (AspirSambaAmount(1, 100, 5, 20) != 5)
    {
        return false;
    }
    if (!ShouldApplyDrainSamba(SambaDaze::Drain, 1) || ShouldApplyAspirSamba(SambaDaze::Aspir, 1, 0))
    {
        return false;
    }
    if (!ShouldApplyHasteSamba(SambaDaze::Haste, 2))
    {
        return false;
    }
    if (DazePowerFromMembership(true, 5) != 5 || DazePowerFromMembership(false, 5) != 0)
    {
        return false;
    }
    if (!WeaponAddEffectPriority(true, 1) || WeaponAddEffectPriority(false, 1))
    {
        return false;
    }
    if (!GripAddEffectGate(true, true, true, true) || GripAddEffectGate(true, true, false, true))
    {
        return false;
    }
    if (!MobPetAddEffectGate(true, 1) || MobPetAddEffectGate(true, 0))
    {
        return false;
    }
    if (EnspellBloodWeapon != 17 || EnspellAuspice != 18)
    {
        return false;
    }
    return true;
}
} // namespace

auto runEnspellHandle1536SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "enspell_handle_1536 self-tests failed\n";
        return false;
    }
    return true;
}
