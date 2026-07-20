require('scripts/actions/mobskills/shuffle')
describe('Shuffle mob skill', function()
    it('requires NM and dispels with erase or no-effect message', function()
        local shuffle = require('scripts/actions/mobskills/shuffle')
        local isNM, effect, message = false, xi.effect.NONE, nil
        local mob = { isNM = function() return isNM end }
        local target = { dispelStatusEffect = function() return effect end }
        local skill = { setMsg = function(_, value) message = value end }
        assert(shuffle.onMobSkillCheck(target, mob, skill) == 1)
        isNM = true; assert(shuffle.onMobSkillCheck(target, mob, skill) == 0)
        assert(shuffle.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.NONE)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
        effect = xi.effect.PROTECT
        assert(shuffle.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.PROTECT)
        assert(message == xi.msg.basic.SKILL_ERASE)
    end)
end)
