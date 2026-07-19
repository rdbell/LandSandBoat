require('scripts/actions/mobskills/counterstance_4')

describe('Counterstance IV mob skill', function()
    it('sets job-ability category, applies Counterstance, emits NONE, and returns Counterstance', function()
        local counterstance = require('scripts/actions/mobskills/counterstance_4')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local category, buff, message = nil, nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        local action = { setCategory = function(_, value) category = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
        end
        assert(counterstance.onMobSkillCheck(nil, mob, skill) == 0)
        assert(counterstance.onMobWeaponSkill(mob, nil, skill, action) == xi.effect.COUNTERSTANCE)
        xi.mobskills.mobBuffMove = originalBuffMove
        assert(category == xi.action.category.JOBABILITY_FINISH)
        assert(buff[1] == mob and buff[2] == xi.effect.COUNTERSTANCE)
        assert(buff[3] == 45 and buff[4] == 0 and buff[5] == 300 and message == xi.msg.basic.NONE)
    end)
end)
