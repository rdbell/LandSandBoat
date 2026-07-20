require('scripts/actions/mobskills/binary_tap')
describe('Binary Tap mob skill', function()
    it('steals up to two DISPELABLE|FOOD effects', function()
        local skill = require('scripts/actions/mobskills/binary_tap')
        local steals, message, flags = 0, nil, nil
        local mob = {
            stealStatusEffect = function(_, target, f)
                flags = f
                steals = steals + 1
                if steals <= 1 then return 10 end
                return 0
            end,
        }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 1)
        assert(flags == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD))
        assert(message == xi.msg.basic.DISAPPEAR_NUM)
        steals, message = 0, nil
        mob.stealStatusEffect = function() return 0 end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
