require('scripts/actions/mobskills/whispers_of_ire_self')
describe('Whispers of Ire Self mob skill', function()
    it('admits when erasable and erases all with DISAPPEAR_NUM', function()
        local skill = require('scripts/actions/mobskills/whispers_of_ire_self')
        local message = nil
        local sk = { setMsg = function(_, v) message = v end }
        local target = { eraseStatusEffect = function() return xi.effect.NONE end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 1)
        target.eraseStatusEffect = function() return xi.effect.SLOW end
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        local mob = { eraseAllStatusEffect = function() return 3 end }
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 3 and message == xi.msg.basic.DISAPPEAR_NUM)
    end)
end)
