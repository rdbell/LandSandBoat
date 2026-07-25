-----------------------------------
-- Pure system tests for Restraint WSD boost dual-wire (slice 6768 / 2764).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/attack ComputeRestraintWSDBoost / ResolveRestraintWSDBoost.
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local p = xi.combat.physical

describe('shouldApplyRestraintBoostFromParams', function()
    it('requires first swing, restraint, and power < 30', function()
        assert(p.shouldApplyRestraintBoostFromParams({
            isFirstSwing = true, hasRestraint = true, powerLessThan30 = true,
        }) == true)
        assert(p.shouldApplyRestraintBoostFromParams({
            isFirstSwing = false, hasRestraint = true, powerLessThan30 = true,
        }) == false)
        assert(p.shouldApplyRestraintBoostFromParams({
            isFirstSwing = true, hasRestraint = false, powerLessThan30 = true,
        }) == false)
        assert(p.shouldApplyRestraintBoostFromParams({
            isFirstSwing = true, hasRestraint = true, powerLessThan30 = false,
        }) == false)
    end)
end)

describe('computeRestraintWSDBoostFromParams', function()
    it('base delay 4000ms ≈ boost 0 sub 62', function()
        local res = p.computeRestraintWSDBoostFromParams({
            weaponDelayMs = 4000, effectPower = 0, effectSubPower = 0,
        })
        assert(res.applies == true)
        assert(res.boostAmount == 0)
        assert(res.newSubPower == 62)
    end)

    it('carries subPower remainder into next floor', function()
        local res = p.computeRestraintWSDBoostFromParams({
            weaponDelayMs = 4000, effectPower = 0, effectSubPower = 50,
        })
        assert(res.boostAmount == 1)
        assert(res.newSubPower == 12)
    end)

    it('jpBonus and enhances only move fractional remainder at this delay', function()
        local jp = p.computeRestraintWSDBoostFromParams({
            weaponDelayMs = 4000, jpBonus = 20,
        })
        assert(jp.boostAmount == 0)
        assert(jp.newSubPower == 74)

        local enh = p.computeRestraintWSDBoostFromParams({
            weaponDelayMs = 4000, enhancesRestraint = 10,
        })
        assert(enh.boostAmount == 0)
        assert(enh.newSubPower == 68)
    end)

    it('caps boost when power + boost would exceed 30', function()
        -- 20000ms + rem 95 → floor ~4; power 28 → boost trimmed to 2
        local res = p.computeRestraintWSDBoostFromParams({
            weaponDelayMs = 20000, effectPower = 28, effectSubPower = 95,
        })
        assert(res.boostAmount == 2)
    end)

    it('integral boost stores remainder 100', function()
        local res = p.computeRestraintWSDBoostFromParams({
            weaponDelayMs = 0, effectSubPower = 200,
        })
        assert(res.boostAmount == 2)
        assert(res.newSubPower == 100)
    end)
end)

describe('resolveRestraintWSDBoostFromParams', function()
    it('gates non-first-swing and at-cap power', function()
        local noFirst = p.resolveRestraintWSDBoostFromParams({
            isFirstSwing = false, hasRestraint = true,
            effectPower = 0, effectSubPower = 0, weaponDelayMs = 4000,
        })
        assert(noFirst.applies == false)

        local atCap = p.resolveRestraintWSDBoostFromParams({
            isFirstSwing = true, hasRestraint = true,
            effectPower = 30, effectSubPower = 40, weaponDelayMs = 4000,
        })
        assert(atCap.applies == false)
        assert(atCap.newSubPower == 40)
    end)

    it('applies when gated on', function()
        local res = p.resolveRestraintWSDBoostFromParams({
            isFirstSwing = true, hasRestraint = true,
            effectPower = 0, effectSubPower = 50, weaponDelayMs = 4000,
        })
        assert(res.applies == true)
        assert(res.boostAmount == 1)
        assert(res.newSubPower == 12)
    end)

    it('pins restraintMaxPower constant', function()
        assert(p.restraintMaxPower == 30)
    end)
end)
