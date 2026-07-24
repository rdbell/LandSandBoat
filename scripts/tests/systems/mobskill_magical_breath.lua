require('scripts/globals/mobskills')

describe('Mob skill TP value', function()
    it('sums skill TP and bonus under the cap', function()
        assert(xi.mobskills.mobSkillTPValue(1000, 0) == 1000)
        assert(xi.mobskills.mobSkillTPValue(0, 0) == 0)
        assert(xi.mobskills.mobSkillTPValue(2999, 1) == 3000)
    end)

    it('hard-caps at 3000', function()
        assert(xi.mobskills.mobSkillTPValue(2500, 600) == 3000)
        assert(xi.mobskills.mobSkillTPValue(3000, 100) == 3000)
    end)
end)

describe('Mob magical dStat', function()
    it('returns zero when the multiplier is not applied', function()
        assert(xi.mobskills.mobMagicalDStat(50, 2, false, false) == 0)
    end)

    it('multiplies a positive delta', function()
        assert(xi.mobskills.mobMagicalDStat(40, 1.5, true, false) == 60)
        assert(xi.mobskills.mobMagicalDStat(40, 2, true, true) == 80)
    end)

    it('penalizes negative dStat for non-avatars', function()
        -- mult 1 → 0.5, force dStat=-1 → floor(-0.5)=-1
        assert(xi.mobskills.mobMagicalDStat(-20, 1, true, false) == -1)
        -- mult 1.5 → 1.0, keep raw → -20
        assert(xi.mobskills.mobMagicalDStat(-20, 1.5, true, false) == -20)
        -- mult 2 → 1.5 → floor(-30)=-30
        assert(xi.mobskills.mobMagicalDStat(-20, 2, true, false) == -30)
    end)

    it('does not penalize avatars for negative dStat', function()
        assert(xi.mobskills.mobMagicalDStat(-20, 1, true, true) == -20)
        assert(xi.mobskills.mobMagicalDStat(-20, 0.5, true, true) == -10)
    end)

    it('clamps to [-65, 999]', function()
        assert(xi.mobskills.mobMagicalDStat(1000, 2, true, false) == 999)
        assert(xi.mobskills.mobMagicalDStat(-1000, 2, true, false) == -65)
    end)
end)

describe('Mob magical base damage', function()
    it('floors the full product and floors at zero', function()
        -- floor((10+5+2)*1.5 + 3 + 1) = floor(29.5) = 29
        assert(xi.mobskills.mobMagicalBaseDamage(10, 5, 2, 1.5, 3, 1) == 29)
        assert(xi.mobskills.mobMagicalBaseDamage(0, 0, 0, 1, -100, 0) == 0)
        assert(xi.mobskills.mobMagicalBaseDamage(50, 0, 0, 1, 0, 0) == 50)
    end)
end)

describe('Mob breath base damage', function()
    it('floors percent of current HP plus flat bonus', function()
        assert(xi.mobskills.mobBreathBaseDamage(1000, 0.10, 5) == 105)
        assert(xi.mobskills.mobBreathBaseDamage(0, 0.5, 10) == 10)
        assert(xi.mobskills.mobBreathBaseDamage(999, 0.1, 0) == 99)
    end)
end)

describe('Mob magical mitigation product', function()
    it('is identity under all-ones multipliers', function()
        assert(xi.mobskills.mobMagicalMitigationProduct(100, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1) == 100)
    end)

    it('floors after each multiply', function()
        -- 100*0.5=50, *1.5=75, *1.1=82 (not 82.5)
        assert(xi.mobskills.mobMagicalMitigationProduct(100, 0.5, 1.5, 1.1, 1, 1, 1, 1, 1, 1, 1) == 82)
    end)

    -- Pin vs hybridMagicDamage: magical keeps SDT on the absorb/heal path.
    it('keeps SDT on the absorb path', function()
        assert(xi.mobskills.mobMagicalMitigationProduct(100, 0.5, 1, 1, 1, 1, 1, 1, -1, 1, 1) == -50)
    end)
end)

describe('Mob breath mitigation product', function()
    it('is identity under the cap', function()
        assert(xi.mobskills.mobBreathMitigationProduct(100, 1, 1, 1, 1, 1, 1, 1, 1, 1, 500) == 100)
    end)

    it('clamps to [0, cap]', function()
        assert(xi.mobskills.mobBreathMitigationProduct(200, 1, 1, 1, 1, 1, 1, 1, 1, 1, 50) == 50)
        assert(xi.mobskills.mobBreathMitigationProduct(100, 1, 1, 1, 1, 1, 1, -1, 1, 1, 500) == 0)
    end)

    it('applies floors before the cap', function()
        -- 100*0.5=50 *2=100, cap 80 → 80
        assert(xi.mobskills.mobBreathMitigationProduct(100, 0.5, 2, 1, 1, 1, 1, 1, 1, 1, 80) == 80)
    end)
end)

describe('Mob processDamage gate', function()
    it('applies only when a hit landed', function()
        assert(xi.mobskills.processDamageApplies(1))
        assert(xi.mobskills.processDamageApplies(3))
        assert(not xi.mobskills.processDamageApplies(0))
        assert(not xi.mobskills.processDamageApplies(-1))
    end)
end)
