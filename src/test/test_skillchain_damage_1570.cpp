#include "test_skillchain_damage_1570.h"

#include "map/skillchain_damage_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace skillchaindamagehelpers;

auto AlmostEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

auto Check() -> bool
{
    // Chain multipliers: corners + invalid.
    if (!AlmostEqual(ChainMultiplier(1, 1), 0.50) || !AlmostEqual(ChainMultiplier(1, 6), 1.00) ||
        !AlmostEqual(ChainMultiplier(2, 3), 1.00) || !AlmostEqual(ChainMultiplier(4, 6), 3.00) ||
        !AlmostEqual(ChainMultiplier(0, 1), 0.0) || !AlmostEqual(ChainMultiplier(5, 1), 0.0) ||
        !AlmostEqual(ChainMultiplier(1, 0), 0.0) || !AlmostEqual(ChainMultiplier(1, 7), 0.0))
    {
        return false;
    }
    for (std::uint8_t level = 1; level <= 4; ++level)
    {
        for (std::uint8_t count = 1; count <= 6; ++count)
        {
            if (ChainMultiplier(level, count) <= 0.0)
            {
                return false;
            }
        }
    }

    // Resistance rank ladder + clamp.
    if (!AlmostEqual(ResRankMultiplier(-3), 1.50) || !AlmostEqual(ResRankMultiplier(0), 1.00) ||
        !AlmostEqual(ResRankMultiplier(4), 0.50) || !AlmostEqual(ResRankMultiplier(11), 0.05) ||
        !AlmostEqual(ResRankMultiplier(-100), 1.50) || !AlmostEqual(ResRankMultiplier(100), 0.05))
    {
        return false;
    }
    if (ClampResRank(-10) != -3 || ClampResRank(20) != 11 || ClampResRank(0) != 0)
    {
        return false;
    }

    // Element membership: singles and multi.
    if (!HasElement(SCLiquefaction, ElementFire) || HasElement(SCLiquefaction, ElementIce) ||
        !HasElement(SCTransfixion, ElementLight) || !HasElement(SCCompression, ElementDark))
    {
        return false;
    }
    {
        const auto grav = ElementsFor(SCGravitation);
        if (grav.size() != 2 || grav[0] != ElementEarth || grav[1] != ElementDark)
        {
            return false;
        }
        const auto light = ElementsFor(SCLight);
        if (light.size() != 4 || light[0] != ElementFire || light[1] != ElementWind ||
            light[2] != ElementThunder || light[3] != ElementLight)
        {
            return false;
        }
        if (!ElementsFor(SCNone).empty() || HasElement(SCNone, ElementFire))
        {
            return false;
        }
    }

    // SelectElement: equal ranks prefer earliest FIRE..DARK; strict lowest wins.
    {
        auto equal = [](std::uint8_t) -> int { return 0; };
        if (SelectElement(SCLiquefaction, equal) != ElementFire)
        {
            return false;
        }
        if (SelectElement(SCGravitation, equal) != ElementEarth)
        {
            return false;
        }
        auto darkLower = [](std::uint8_t el) -> int {
            return el == ElementDark ? 2 : 5;
        };
        if (SelectElement(SCGravitation, darkLower) != ElementDark)
        {
            return false;
        }
        auto lightLowest = [](std::uint8_t el) -> int {
            return el == ElementLight ? -3 : 0;
        };
        if (SelectElement(SCLight, lightLowest) != ElementLight)
        {
            return false;
        }
        if (SelectElement(SCNone, equal) != ElementNone)
        {
            return false;
        }
    }

    // Product early-outs.
    {
        ProductParams p{};
        p.hasEffect  = true;
        p.type       = SCLiquefaction;
        p.level      = 1;
        p.count      = 1;
        p.element    = ElementFire;
        p.baseDamage = 1000;
        p.bonusMult = p.damageMult = p.dayWeatherMult = p.staffMult = p.affinityMult = 1.0;
        p.magicTakenMult = p.inninMult = p.sengikoriMult = p.absorbMult = 1.0;

        {
            const auto got = Product(p);
            if (got.damage != 500 || !got.applied || !got.consumeSengikori || got.element != ElementFire)
            {
                return false;
            }
        }
        {
            auto q       = p;
            q.hasEffect  = false;
            const auto r = Product(q);
            if (r.applied || r.damage != 0)
            {
                return false;
            }
        }
        {
            auto q    = p;
            q.type    = SCNone;
            if (Product(q).applied)
            {
                return false;
            }
        }
        {
            auto q   = p;
            q.level  = 0;
            if (Product(q).applied)
            {
                return false;
            }
            q.level = 5;
            if (Product(q).applied)
            {
                return false;
            }
        }
        {
            auto q   = p;
            q.count  = 0;
            if (Product(q).applied)
            {
                return false;
            }
            q.count = 7;
            if (Product(q).applied)
            {
                return false;
            }
        }
        {
            auto q     = p;
            q.element  = ElementNone;
            if (Product(q).applied)
            {
                return false;
            }
        }
        {
            auto q       = p;
            q.nullified  = true;
            const auto r = Product(q);
            if (r.damage != 0 || r.applied || r.consumeSengikori || r.element != ElementFire)
            {
                return false;
            }
        }
    }

    // Abs base + level/count floors.
    {
        ProductParams p{};
        p.hasEffect  = true;
        p.type       = SCLight;
        p.level      = 3;
        p.count      = 1;
        p.element    = ElementFire;
        p.baseDamage = -800;
        p.bonusMult = p.damageMult = p.dayWeatherMult = p.staffMult = p.affinityMult = 1.0;
        p.magicTakenMult = p.inninMult = p.sengikoriMult = p.absorbMult = 1.0;
        if (Product(p).damage != 800)
        {
            return false;
        }
        p.level = 4;
        p.count = 6;
        p.baseDamage = 1000;
        if (Product(p).damage != 3000)
        {
            return false;
        }
    }

    // Bonus + MAGIC_DAMAGE step.
    {
        ProductParams p{};
        p.hasEffect    = true;
        p.type         = SCLiquefaction;
        p.level        = 1;
        p.count        = 1;
        p.element      = ElementFire;
        p.baseDamage   = 1000;
        p.bonusMult    = 1.2;
        p.magicDamage  = 50;
        p.damageMult = p.dayWeatherMult = p.staffMult = p.affinityMult = 1.0;
        p.magicTakenMult = p.inninMult = p.sengikoriMult = p.absorbMult = 1.0;
        // floor(1000*0.5)=500; floor(500*1.2)=600 + 50 = 650
        if (Product(p).damage != 650)
        {
            return false;
        }
    }

    // Full floor-product golden chain (matches Go skillchaindmg TestProductFullChainGolden).
    {
        ProductParams p{};
        p.hasEffect      = true;
        p.type           = SCFusion;
        p.level          = 2;
        p.count          = 3;
        p.element        = ElementFire;
        p.baseDamage     = 1000;
        p.bonusMult      = 1.10;
        p.damageMult     = 1.05;
        p.dayWeatherMult = 1.10;
        p.staffMult      = 1.05;
        p.affinityMult   = 1.02;
        p.magicTakenMult = 0.90;
        p.inninMult      = 1.05;
        p.sengikoriMult  = 1.10;
        p.absorbMult     = 1.0;
        p.resRank        = 0;
        p.magicDamage    = 25;

        double final = 1000.0;
        final        = std::floor(final * 1.00);
        final        = std::floor(final * 1.10) + 25;
        final        = std::floor(final * 1.05);
        final        = std::floor(final * 1.10);
        final        = std::floor(final * 1.05);
        final        = std::floor(final * 1.02);
        final        = std::floor(final * ResRankMultiplier(0));
        final        = std::floor(final * 0.90);
        final        = std::floor(final * 1.05);
        final        = std::floor(final * 1.10);
        final        = std::floor(final * 1.00);
        const auto got = Product(p);
        if (got.damage != static_cast<std::int32_t>(final) || !got.applied || !got.consumeSengikori)
        {
            return false;
        }
    }

    // Absorb sign flip.
    {
        ProductParams p{};
        p.hasEffect  = true;
        p.type       = SCCompression;
        p.level      = 1;
        p.count      = 2;
        p.element    = ElementDark;
        p.baseDamage = 500;
        p.absorbMult = -1.0;
        p.bonusMult = p.damageMult = p.dayWeatherMult = p.staffMult = p.affinityMult = 1.0;
        p.magicTakenMult = p.inninMult = p.sengikoriMult = 1.0;
        // floor(500*0.6)=300 * -1 = -300
        const auto got = Product(p);
        if (got.damage != -300 || !got.applied)
        {
            return false;
        }
    }

    // Res-rank bands.
    {
        ProductParams p{};
        p.hasEffect  = true;
        p.type       = SCTransfixion;
        p.level      = 3;
        p.count      = 1;
        p.element    = ElementLight;
        p.baseDamage = 1000;
        p.bonusMult = p.damageMult = p.dayWeatherMult = p.staffMult = p.affinityMult = 1.0;
        p.magicTakenMult = p.inninMult = p.sengikoriMult = p.absorbMult = 1.0;
        p.resRank = -3;
        if (Product(p).damage != 1500)
        {
            return false;
        }
        p.resRank = 4;
        if (Product(p).damage != 500)
        {
            return false;
        }
        p.resRank = 11;
        if (Product(p).damage != 50)
        {
            return false;
        }
        p.resRank = 99;
        if (Product(p).damage != 50)
        {
            return false;
        }
    }

    // Post-product clamps used by TakeSkillchainDamage host.
    if (ClampSCDamage(-10) != 0 || ClampSCDamage(0) != 0 || ClampSCDamage(99999) != 99999 ||
        ClampSCDamage(100000) != 99999)
    {
        return false;
    }
    if (ApplyPhalanx(100, 30) != 70 || ApplyPhalanx(10, 30) != 0 || ApplyPhalanx(0, 5) != 0 ||
        ApplyPhalanx(-5, 5) != -5)
    {
        return false;
    }

    return true;
}
} // namespace

auto runSkillchainDamage1570SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "skillchain_damage_1570 self-tests failed\n";
        return false;
    }
    return true;
}
