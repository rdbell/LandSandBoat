require('scripts/actions/mobskills/heliovoid')
describe('Heliovoid mob skill', function()
    it('drains one effect or reports miss', function()
        local skill = require('scripts/actions/mobskills/heliovoid')
        local msg, left = nil, 1
        local mob = {
            stealStatusEffect = function()
                if left > 0 then left = left - 1; return 1 end
                return 0
            end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 1)
        assert(msg == xi.msg.basic.EFFECT_DRAINED)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.SKILL_MISS)
    end)
end)
