require('scripts/globals/mobskills')

local function plan(shadows, aoe, conal, attackType)
    return xi.mobskills.shadowConsumptionPlan(shadows, aoe, conal, attackType)
end

describe('Mob skill shadow consumption plan', function()
    it('wipes copy image and blink always', function()
        local p = plan(xi.mobskills.shadowBehavior.WIPE_SHADOWS, false, false, xi.attackType.MAGICAL)
        assert(p.delCopyImage and p.delBlink)
        assert(not p.delThirdEye) -- magical: no Third Eye
        assert(not p.attemptAbsorb)
    end)

    it('wipes third eye only for physical or ranged', function()
        local phys = plan(xi.mobskills.shadowBehavior.WIPE_SHADOWS, false, false, xi.attackType.PHYSICAL)
        assert(phys.delThirdEye)
        local ranged = plan(xi.mobskills.shadowBehavior.WIPE_SHADOWS, false, false, xi.attackType.RANGED)
        assert(ranged.delThirdEye)
        local breath = plan(xi.mobskills.shadowBehavior.WIPE_SHADOWS, false, false, xi.attackType.BREATH)
        assert(not breath.delThirdEye)
    end)

    -- AoE/conal strip Blink even when the skill ignores shadow counts.
    it('strips blink on AoE ignore without attempting absorb', function()
        local p = plan(xi.mobskills.shadowBehavior.IGNORE_SHADOWS, true, false, xi.attackType.PHYSICAL)
        assert(p.delBlink and p.delThirdEye)
        assert(not p.delCopyImage)
        assert(not p.attemptAbsorb)
    end)

    it('strips blink on conal magical without third eye', function()
        local p = plan(xi.mobskills.shadowBehavior.IGNORE_SHADOWS, false, true, xi.attackType.MAGICAL)
        assert(p.delBlink)
        assert(not p.delThirdEye)
        assert(not p.attemptAbsorb)
    end)

    it('plans a single-shadow absorb for normal skills', function()
        local p = plan(xi.mobskills.shadowBehavior.NUMSHADOWS_1, false, false, xi.attackType.PHYSICAL)
        assert(p.attemptAbsorb)
        assert(p.attemptedRemoval == 1)
        assert(not p.useAoEMitigation)
        assert(not p.delBlink and not p.delCopyImage and not p.delThirdEye)
    end)

    it('enables AoE mitigation for multi-shadow AoE skills', function()
        local p = plan(xi.mobskills.shadowBehavior.NUMSHADOWS_3, true, false, xi.attackType.PHYSICAL)
        assert(p.attemptAbsorb and p.useAoEMitigation)
        assert(p.attemptedRemoval == 3)
        assert(p.delBlink and p.delThirdEye)
    end)

    it('does not absorb on wipe even when AoE', function()
        local p = plan(xi.mobskills.shadowBehavior.WIPE_SHADOWS, true, false, xi.attackType.PHYSICAL)
        assert(not p.attemptAbsorb)
        assert(p.delCopyImage and p.delBlink and p.delThirdEye)
    end)
end)
