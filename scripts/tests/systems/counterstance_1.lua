require('scripts/actions/mobskills/counterstance_1')

describe('Counterstance I mob skill', function()
    it('allows use, applies Counterstance, emits NONE, and returns Counterstance', function()
        local counterstance = require('scripts/actions/mobskills/counterstance_1')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
        end
        assert(counterstance.onMobSkillCheck(nil, mob, skill) == 0)
        assert(counterstance.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.COUNTERSTANCE)
        xi.mobskills.mobBuffMove = originalBuffMove
        assert(buff[1] == mob and buff[2] == xi.effect.COUNTERSTANCE)
        assert(buff[3] == 45 and buff[4] == 0 and buff[5] == 300 and message == xi.msg.basic.NONE)
    end)
end)
