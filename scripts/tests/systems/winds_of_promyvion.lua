require('scripts/actions/mobskills/winds_of_promyvion')
describe('Winds of Promyvion mob skill', function()
    it('erases one effect or reports no effect', function()
        local skill = require('scripts/actions/mobskills/winds_of_promyvion')
        local message = nil
        local sk = { setMsg = function(_, v) message = v end }
        local target = { eraseStatusEffect = function() return xi.effect.NONE end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.NONE)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
        target.eraseStatusEffect = function() return xi.effect.SLOW end
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.SLOW)
        assert(message == xi.msg.basic.SKILL_ERASE)
    end)
end)
