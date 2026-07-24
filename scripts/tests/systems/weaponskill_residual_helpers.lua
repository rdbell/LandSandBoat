require('scripts/globals/weaponskills')

describe('Weaponskill first-hit rate and target HP helpers', function()
    it('adds one hundred accuracy for the first-hit rate', function()
        assert(xi.weaponskills.firstHitAccuracyBonus(0) == 100)
        assert(xi.weaponskills.firstHitAccuracyBonus(50) == 150)
    end)

    it('swaps first-hit rate only when not Jump and a rate is present', function()
        assert(xi.weaponskills.shouldUseFirstHitRate(false, true))
        assert(not xi.weaponskills.shouldUseFirstHitRate(true, true))
        assert(not xi.weaponskills.shouldUseFirstHitRate(false, false))
    end)

    it('adds stoneskin mod to target HP for the multi-hit stop gate', function()
        assert(xi.weaponskills.weaponskillTargetHp(1000, 200) == 1200)
        assert(xi.weaponskills.weaponskillTargetHp(0, 0) == 0)
    end)
end)

describe('Weaponskill multi and ammo helpers', function()
    it('forces zero mainhand multis on ranged weaponskills', function()
        assert(xi.weaponskills.initialMainhandMultis(true, 5) == 0)
        assert(xi.weaponskills.initialMainhandMultis(false, 5) == 5)
        assert(xi.weaponskills.initialMainhandMultis(false, 0) == 0)
    end)

    it('starts multi-proc count at one when any mainhand multi exists', function()
        assert(xi.weaponskills.initialMultiProcCount(0) == 0)
        assert(xi.weaponskills.initialMultiProcCount(1) == 1)
        assert(xi.weaponskills.initialMultiProcCount(3) == 1)
    end)

    it('counts ammo used as one when shouldUseAmmo is true', function()
        assert(xi.weaponskills.useAmmoCount(true) == 1)
        assert(xi.weaponskills.useAmmoCount(false) == 0)
    end)
end)

describe('Weaponskill magic skill and mitigation floors', function()
    it('detects archery and marksmanship as ranged magic WS', function()
        assert(xi.weaponskills.isRangedMagicWeaponskill(xi.skill.ARCHERY))
        assert(xi.weaponskills.isRangedMagicWeaponskill(xi.skill.MARKSMANSHIP))
        assert(not xi.weaponskills.isRangedMagicWeaponskill(xi.skill.SWORD))
        assert(not xi.weaponskills.isRangedMagicWeaponskill(nil))
    end)

    it('floors resist then damage adjustment', function()
        -- floor(floor(100 * 0.5) * 0.75) = floor(50 * 0.75) = 37
        assert(xi.weaponskills.magicMitigationFloors(100, 0.5, 0.75) == 37)
        assert(xi.weaponskills.magicMitigationFloors(100, 1, 1) == 100)
    end)

    it('floors hybrid absorb and nullify only when positive', function()
        assert(xi.weaponskills.hybridMagicAbsorbNullify(0, 0.5, 0.5) == 0)
        assert(xi.weaponskills.hybridMagicAbsorbNullify(-10, 0.5, 0.5) == -10)
        -- floor(100 * 0.5) = 50; floor(50 * 0.5) = 25
        assert(xi.weaponskills.hybridMagicAbsorbNullify(100, 0.5, 0.5) == 25)
    end)

    it('multiplies magic WS absorb and nullify without intermediate floors', function()
        -- 100 * 0.5 * 0.5 = 25
        assert(xi.weaponskills.magicWeaponskillAbsorbNullify(100, 0.5, 0.5) == 25)
        -- 101 * 0.5 * 0.5 = 25.25 (no floor)
        assert(xi.weaponskills.magicWeaponskillAbsorbNullify(101, 0.5, 0.5) == 25.25)
    end)
end)
