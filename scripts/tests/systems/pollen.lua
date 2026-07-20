require('scripts/actions/mobskills/pollen')

describe('Pollen mob skill', function()
    it('heals with base potency for non-NM and NM bonus range for NMs', function()
        local pollen = require('scripts/actions/mobskills/pollen')
        local heal, random = xi.mobskills.mobHealMove, math.random
        local amount, message, isNM = nil, nil, false
        local mob = {
            isNM = function() return isNM end,
            getMaxHP = function() return 1024 end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobHealMove = function(_, value)
            amount = value
            return value
        end
        isNM = false
        assert(pollen.onMobSkillCheck({}, mob, skill) == 0)
        assert(pollen.onMobWeaponSkill(mob, {}, skill, {}) == 147)
        assert(amount == 147 and message == xi.msg.basic.SELF_HEAL)
        isNM = true
        math.random = function(low, high)
            assert(low == 0 and high == 294)
            return 294
        end
        assert(pollen.onMobWeaponSkill(mob, {}, skill, {}) == 441)
        xi.mobskills.mobHealMove, math.random = heal, random
        assert(amount == 441)
    end)
end)
