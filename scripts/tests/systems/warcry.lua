require('scripts/actions/mobskills/warcry')

describe('Warcry mob skill', function()
    it('sets the job-ability action category, applies its fixed buff, and forwards its message', function()
        local warcry = require('scripts/actions/mobskills/warcry')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local category, buff, message = nil, nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        local action = { setCategory = function(_, value) category = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end

        assert(warcry.onMobSkillCheck(nil, mob, skill) == 0)
        assert(warcry.onMobWeaponSkill(mob, nil, skill, action) == xi.effect.WARCRY)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(category == xi.action.category.JOBABILITY_FINISH)
        assert(buff[1] == mob and buff[2] == xi.effect.WARCRY)
        assert(buff[3] == 8 and buff[4] == 0 and buff[5] == 30)
        assert(message == 123)
    end)
end)
