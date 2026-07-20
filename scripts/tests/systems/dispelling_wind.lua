require('scripts/actions/mobskills/dispelling_wind')

describe('Dispelling Wind mob skill', function()
    it('makes its random number of dispel attempts and selects success or no-effect messages', function()
        local wind = require('scripts/actions/mobskills/dispelling_wind')
        local random = math.random
        local attempts, message = 0, nil
        local effects = { xi.effect.PROTECT, xi.effect.NONE, xi.effect.SHELL }
        local target = { dispelStatusEffect = function() attempts = attempts + 1; return effects[attempts] end }
        local skill = { setMsg = function(_, value) message = value end }
        math.random = function(low, high) assert(low == 1 and high == 3); return 3 end

        assert(wind.onMobSkillCheck({}, {}, {}) == 0)
        assert(wind.onMobWeaponSkill({}, target, skill, {}) == 2)
        assert(attempts == 3 and message == xi.msg.basic.DISAPPEAR_NUM)

        attempts, message = 0, nil
        effects = { xi.effect.NONE }
        math.random = function() return 1 end
        assert(wind.onMobWeaponSkill({}, target, skill, {}) == 0)
        math.random = random
        assert(attempts == 1 and message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
