require('scripts/globals/weaponskills')

describe('Weaponskill ignoresDefense', function()
    it('is true only when the ignoredDefense table is present', function()
        assert(xi.weaponskills.ignoresDefense(true))
        assert(not xi.weaponskills.ignoresDefense(false))
    end)
end)

describe('Weaponskill attack type is physical', function()
    it('matches PHYSICAL only', function()
        assert(xi.weaponskills.attackTypeIsPhysical(xi.attackType.PHYSICAL))
        assert(not xi.weaponskills.attackTypeIsPhysical(xi.attackType.RANGED))
    end)
end)

describe('Weaponskill single-hit block reduction input', function()
    it('does not floor the pre-block product', function()
        assert(xi.weaponskills.singleHitBlockReductionInput(10, 2, 1.5, 2) == 36)
        assert(xi.weaponskills.singleHitBlockReductionInput(10, 0, 1.15, 1) == 11.5)
    end)
end)

describe('Weaponskill single-hit guard plan', function()
    it('applies only to physical guarded hits', function()
        local plan = xi.weaponskills.planSingleHitGuard(false, true, 2.5)
        assert(not plan.applyGuard and plan.newPDIF == 2.5)

        plan = xi.weaponskills.planSingleHitGuard(true, false, 2.5)
        assert(not plan.applyGuard)

        plan = xi.weaponskills.planSingleHitGuard(true, true, 2.5)
        assert(plan.applyGuard and plan.newPDIF == 1.5 and plan.guardedHitsDelta == 1)

        plan = xi.weaponskills.planSingleHitGuard(true, true, 0.5)
        assert(plan.newPDIF == 0)
    end)
end)

describe('Weaponskill single-hit success plan', function()
    it('increments hitsLanded and sticky-ors critical', function()
        local plan = xi.weaponskills.planSingleHitSuccess(2, 0, 2.0, false, true, true, false)
        assert(plan.hitsLanded == 3 and plan.guardedHits == 0)
        assert(plan.pdif == 2.0 and plan.criticalHit)

        plan = xi.weaponskills.planSingleHitSuccess(0, 1, 2.0, true, false, true, true)
        assert(plan.hitsLanded == 1 and plan.guardedHits == 2)
        assert(plan.pdif == 1.0 and plan.criticalHit)
    end)

    it('ignores guard injects for non-physical hits', function()
        local plan = xi.weaponskills.planSingleHitSuccess(0, 0, 1.5, false, false, false, true)
        assert(plan.hitsLanded == 1 and plan.guardedHits == 0 and plan.pdif == 1.5)
    end)
end)

describe('Weaponskill offhand multi landed plan', function()
    it('requires positive damage', function()
        assert(not xi.weaponskills.planOffhandMultiLanded(0, true).applies)

        local plan = xi.weaponskills.planOffhandMultiLanded(10, false)
        assert(plan.applies and plan.trySkillUp and not plan.addJumpTP)

        plan = xi.weaponskills.planOffhandMultiLanded(10, true)
        assert(plan.addJumpTP)
    end)
end)
