require('scripts/actions/mobskills/remove_blindness')
describe('Remove Blindness mob skill', function()
    it('erases BLINDNESS when present and reports no effect otherwise', function()
        local skillObj = require('scripts/actions/mobskills/remove_blindness')
        local message, erased = nil, true
        local target = {
            delStatusEffect = function(_, effect)
                assert(effect == xi.effect.BLINDNESS)
                return erased
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(skillObj.onMobSkillCheck(target, {}, skill) == 0)
        assert(skillObj.onMobWeaponSkill({}, target, skill, {}) == xi.effect.BLINDNESS)
        assert(message == xi.msg.basic.SKILL_ERASE)
        erased = false
        assert(skillObj.onMobWeaponSkill({}, target, skill, {}) == xi.effect.BLINDNESS)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
