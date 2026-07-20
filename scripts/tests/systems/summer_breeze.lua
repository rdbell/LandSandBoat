require('scripts/actions/mobskills/summer_breeze')
describe('Summer Breeze mob skill', function()
    it('erases a status or falls back to Regain', function()
        local skill = require('scripts/actions/mobskills/summer_breeze')
        local buff = xi.mobskills.mobBuffMove
        local message, buffParams = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { eraseStatusEffect = function() return xi.effect.SLOW end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.SLOW and message == xi.msg.basic.SKILL_ERASE)
        mob.eraseStatusEffect = function() return xi.effect.NONE end
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            buffParams = { m, effect, power, tick, duration }
            return 101
        end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.REGAIN and message == 101)
        xi.mobskills.mobBuffMove = buff
        assert(buffParams[2] == xi.effect.REGAIN and buffParams[3] == 10 and buffParams[4] == 3 and buffParams[5] == 60)
    end)
end)
