require('scripts/actions/mobskills/spirit_tap')
describe('Spirit Tap mob skill', function()
    it('rejects notorious and steals buff with drained message', function()
        local tap = require('scripts/actions/mobskills/spirit_tap')
        local isNM, stole, message = true, 0, nil
        local mob = {
            isMobType = function(_, t) return isNM and t == xi.mobType.NOTORIOUS end,
            stealStatusEffect = function() return stole end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(tap.onMobSkillCheck({}, mob, skill) == 1)
        isNM = false; assert(tap.onMobSkillCheck({}, mob, skill) == 0)
        assert(tap.onMobWeaponSkill(mob, {}, skill, {}) == 1)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
        stole = 3
        assert(tap.onMobWeaponSkill(mob, {}, skill, {}) == 1)
        assert(message == xi.msg.basic.EFFECT_DRAINED)
    end)
end)
