require('scripts/actions/mobskills/level_5_petrify')

describe('Level 5 Petrify mob skill', function()
    it('only petrifies targets with main levels divisible by five', function()
        local petrify = require('scripts/actions/mobskills/level_5_petrify')
        local duration, status = xi.mobskills.calculateDuration, xi.mobskills.mobStatusEffectMove
        local call, message = nil, nil
        local mob = { getTP = function() return 1500 end }
        local target = { getMainLvl = function() return 50 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.calculateDuration = function(tp, minimum, maximum)
            assert(tp == 1500 and minimum == 15 and maximum == 60)
            return 37.5
        end
        xi.mobskills.mobStatusEffectMove = function(...) call = { ... }; return 456 end

        assert(petrify.onMobSkillCheck(target, mob, skill) == 0)
        assert(petrify.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.PETRIFICATION)
        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.PETRIFICATION and call[4] == 1 and call[5] == 0 and call[6] == 37.5 and message == 456)

        target.getMainLvl = function() return 51 end
        call, message = nil, nil
        assert(petrify.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.PETRIFICATION)
        xi.mobskills.calculateDuration, xi.mobskills.mobStatusEffectMove = duration, status
        assert(call == nil and message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
