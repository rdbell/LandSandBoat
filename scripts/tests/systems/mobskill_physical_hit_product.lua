require('scripts/globals/mobskills')

describe('Mob skill physical hit defense outcome', function()
    it('prefers parry over guard', function()
        local o = xi.mobskills.physicalHitDefenseOutcome(true, true)
        assert(o.early and o.hitParried and not o.hitGuarded)
        assert(o.missType == 'Parried')
    end)

    it('reports guard when only guard applies', function()
        local o = xi.mobskills.physicalHitDefenseOutcome(false, true)
        assert(o.early and o.hitGuarded and not o.hitParried)
        assert(o.missType == 'Guarded')
    end)

    it('continues when neither applies', function()
        local o = xi.mobskills.physicalHitDefenseOutcome(false, false)
        assert(not o.early)
        assert(o.missType == nil)
    end)
end)

describe('Mob skill crit roll', function()
    it('succeeds when the roll is at or below rate times one thousand', function()
        assert(xi.mobskills.critRollSucceeds(150, 0.15))
        assert(xi.mobskills.critRollSucceeds(1, 0.001))
        assert(not xi.mobskills.critRollSucceeds(151, 0.15))
        assert(not xi.mobskills.critRollSucceeds(1000, 0))
    end)

    it('treats full rate as always succeeding on a legal roll', function()
        assert(xi.mobskills.critRollSucceeds(1000, 1.0))
        assert(xi.mobskills.critRollSucceeds(1, 1.0))
    end)
end)

describe('Mob skill physical hit raw damage', function()
    it('floors base times pDif', function()
        assert(xi.mobskills.physicalHitRawDamage(100, 1.5, false, 0, 1, 1, false, 0) == 150)
        assert(xi.mobskills.physicalHitRawDamage(101, 1.5, false, 0, 1, 1, false, 0) == 151)
    end)

    it('subtracts block reduction before SDT without an intermediate floor', function()
        -- floor(100 * 1) = 100, -30 = 70, * 0.5 = 35
        assert(xi.mobskills.physicalHitRawDamage(100, 1, true, 30, 0.5, 1, false, 0) == 35)
    end)

    it('applies SDT and damage adjustment with intermediate floors', function()
        -- floor(100 * 2) = 200, * 0.75 = 150, * 0.5 = 75
        assert(xi.mobskills.physicalHitRawDamage(100, 2, false, 0, 0.75, 0.5, false, 0) == 75)
    end)

    it('applies avatar BP_DAMAGE after SDT and adjustment', function()
        -- floor(100*1)=100, *1,*1, then floor(100 + 100*50/100)=150
        assert(xi.mobskills.physicalHitRawDamage(100, 1, false, 0, 1, 1, true, 50) == 150)
        -- non-avatar ignores BP mod
        assert(xi.mobskills.physicalHitRawDamage(100, 1, false, 0, 1, 1, false, 50) == 100)
    end)
end)

describe('Mob skill shield mastery TP gate', function()
    it('requires mastery block and residual damage past stoneskin power', function()
        assert(xi.mobskills.shieldMasteryTPApplies(true, 100, 50))
        assert(not xi.mobskills.shieldMasteryTPApplies(true, 50, 50))
        assert(not xi.mobskills.shieldMasteryTPApplies(true, 40, 50))
        assert(not xi.mobskills.shieldMasteryTPApplies(false, 100, 0))
    end)
end)
