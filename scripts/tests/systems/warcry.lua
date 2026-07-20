require('scripts/actions/mobskills/warcry')
describe('Warcry mob skill', function()
    it('sets JOBABILITY_FINISH and buffs WARCRY power 8', function()
        local skill = require('scripts/actions/mobskills/warcry')
        local buff = xi.mobskills.mobBuffMove
        local message, category, buffParams = nil, nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local action = { setCategory = function(_, c) category = c end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            buffParams = { effect, power, tick, duration }
            return 101
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, action) == xi.effect.WARCRY and message == 101)
        xi.mobskills.mobBuffMove = buff
        assert(category == xi.action.category.JOBABILITY_FINISH)
        assert(buffParams[1] == xi.effect.WARCRY and buffParams[2] == 8 and buffParams[4] == 30)
    end)
end)
