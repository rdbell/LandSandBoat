require('scripts/actions/mobskills/trinary_tap')
describe('Trinary Tap mob skill', function()
    it('steals up to three buffs and messages by count', function()
        local skill = require('scripts/actions/mobskills/trinary_tap')
        local message, flags, n = nil, nil, 0
        local sk = { setMsg = function(_, v) message = v end }
        local mob = {
            isMobType = function(_, t) return t == xi.mobType.NOTORIOUS end,
            stealStatusEffect = function(_, target, f)
                flags = f
                n = n + 1
                if n <= 2 then return 10 end
                return 0
            end,
        }
        local target = { hasStatusEffect = function() return false end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 2 and message == xi.msg.basic.DISAPPEAR_NUM)
        assert(flags == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD) and n == 3)
        n = 0
        mob.stealStatusEffect = function() return 0 end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 0 and message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
