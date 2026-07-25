-----------------------------------
-- Pure system tests for defender TP-gain dual-wire helpers (slice 6682).
-- Calls production xi.combat.tp pure exports.
-- Goldens for modifiers (except dAGI) match internal/tpgain (0852).
-- dAGI pure keeps LSB production operator-precedence form (always ~1 for
-- realistic dAGI); Go DAGIModifier uses the documented intended curve.
-----------------------------------

require('scripts/globals/combat/tp')

local tp = xi.combat.tp

describe('Defender TP-gain constants', function()
    it('pins subtle / dAGI / base pins', function()
        assert(tp.subtleBlowICap == 50)
        assert(tp.subtleBlowModMin == 0.25)
        assert(tp.dagiModMin == 0.5)
        assert(tp.dagiModMax == 1.0)
        assert(tp.magicBaseMob == 100)
        assert(tp.magicBaseNonMob == 50)
        assert(tp.physicalMobBaseBonus == 30)
        assert(math.abs(tp.physicalNonMobShare - (1 / 3)) < 1e-12)
    end)
end)

describe('Defender TP-gain inhibit and store modifiers', function()
    it('scales inhibit and store without caps', function()
        assert(tp.inhibitTPModifier(0) == 1.0)
        assert(math.abs(tp.inhibitTPModifier(25) - 0.75) < 1e-12)
        assert(math.abs(tp.inhibitTPModifier(150) - (-0.5)) < 1e-12)
        assert(tp.storeTPModifier(0) == 1.0)
        assert(math.abs(tp.storeTPModifier(50) - 1.5) < 1e-12)
    end)
end)

describe('Defender TP-gain subtle blow', function()
    it('caps Subtle Blow I at 50 and sums II', function()
        assert(tp.subtleBlowI(40, 5) == 45)
        assert(tp.subtleBlowI(40, 20) == 50)
        assert(tp.subtleBlowI(60, 0) == 50)
        assert(tp.subtleBlowII(10, 5) == 15)
    end)

    it('combines I and II with 0.25 floor', function()
        assert(tp.subtleBlowModifier(0, 0) == 1.0)
        assert(tp.subtleBlowModifier(50, 0) == 0.5)
        assert(tp.subtleBlowModifier(50, -100) == 0.25)
        -- I cap 50, II 15 → (100-50+15)/100 = 0.65
        local i = tp.subtleBlowI(30, 25)
        local ii = tp.subtleBlowII(10, 5)
        assert(math.abs(tp.subtleBlowModifier(i, ii) - 0.65) < 1e-12)
    end)
end)

describe('Defender TP-gain dagiModifier production form', function()
    it('uses operator-precedence form (≈1 for realistic dAGI)', function()
        -- 200 - (dAGI+30)/200 is always clamped to 1 for ordinary combat dAGI
        assert(tp.dagiModifier(-30) == 1.0)
        assert(tp.dagiModifier(0) == 1.0)
        assert(tp.dagiModifier(70) == 1.0)
        assert(tp.dagiModifier(200) == 1.0)
        -- Explicit product check at dAGI 70: 200 - 100/200 = 199.5 → clamp 1
        assert(math.abs((200 - (70 + 30) / 200) - 199.5) < 1e-12)
    end)
end)

describe('Defender TP-gain tandemBlowBonus', function()
    it('selects master vs self power when tandem is active', function()
        assert(tp.tandemBlowBonus(false, true, 20, 10) == 0)
        assert(tp.tandemBlowBonus(true, true, 20, 10) == 20)
        assert(tp.tandemBlowBonus(true, false, 20, 10) == 10)
    end)
end)

describe('Defender TP-gain physicalTPGain', function()
    it('player→mob uses base+30 product', function()
        -- production dAGI mod is 1; 100+30 → 130
        assert(tp.physicalTPGain({
            baseTPGain = 100,
            targetIsMob = true,
            actorIsMob = false,
            dAGI = 70,
        }) == 130)

        -- SB I 50 → 0.5; 130 * 0.5 = 65
        assert(tp.physicalTPGain({
            baseTPGain = 100,
            targetIsMob = true,
            actorIsMob = false,
            dAGI = -30,
            subtleBlow = 50,
        }) == 65)

        -- inhibit 50 → 0.5; store 50 → 1.5; 130 * 0.5 * 1.5 = 97.5 → 97
        assert(tp.physicalTPGain({
            baseTPGain = 100,
            targetIsMob = true,
            actorIsMob = false,
            dAGI = -30,
            inhibitTP = 50,
            storeTP = 50,
        }) == 97)
    end)

    it('non-player→mob path uses base/3 and ignores dAGI', function()
        assert(tp.physicalTPGain({
            baseTPGain = 90,
            targetIsMob = false,
            actorIsMob = false,
            dAGI = 70,
        }) == 30)

        -- mob vs mob (charm)
        assert(tp.physicalTPGain({
            baseTPGain = 90,
            targetIsMob = true,
            actorIsMob = true,
            dAGI = 70,
        }) == 30)

        -- 100 * 0.5 * 1/3 = 16.666 → 16
        assert(tp.physicalTPGain({
            baseTPGain = 100,
            targetIsMob = false,
            subtleBlow = 50,
        }) == 16)
    end)
end)

describe('Defender TP-gain magicalTPGain', function()
    it('uses 100 base for mob and 50 for non-mob', function()
        -- production dAGI mod is 1
        assert(tp.magicalTPGain({ targetIsMob = true, dAGI = 70 }) == 100)
        assert(tp.magicalTPGain({ targetIsMob = false, dAGI = 70 }) == 50)
        assert(tp.magicalTPGain({ targetIsMob = false, subtleBlow = 50 }) == 25)
        -- mob + SB 50 → 50
        assert(tp.magicalTPGain({ targetIsMob = true, dAGI = 20, subtleBlow = 50 }) == 50)
    end)
end)

describe('Defender TP-gain spellTP Occult Acumen', function()
    it('gates and products', function()
        assert(tp.spellTP({
            isPC = false, skillEligible = true, mpCost = 100, occultAcumen = 50,
        }) == 0)
        assert(tp.spellTP({
            isPC = true, meikyoShisui = true, skillEligible = true, mpCost = 100, occultAcumen = 50,
        }) == 0)
        assert(tp.spellTP({
            isPC = true, skillEligible = false, mpCost = 100, occultAcumen = 50,
        }) == 0)
        -- 100 * 0.5 * 1.0 = 50
        assert(tp.spellTP({
            isPC = true, skillEligible = true, mpCost = 100, occultAcumen = 50, storeTP = 0,
        }) == 50)
        -- 80 * 0.25 * 1.5 = 30
        assert(tp.spellTP({
            isPC = true, skillEligible = true, mpCost = 80, occultAcumen = 25, storeTP = 50,
        }) == 30)
    end)
end)
