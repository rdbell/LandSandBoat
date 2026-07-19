require('scripts/actions/mobskills/cocoon')

describe('Cocoon mob skill', function()
    it('allows use, applies the fixed Defense Boost buff, and forwards its message', function()
        local cocoon = require('scripts/actions/mobskills/cocoon')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end
        assert(cocoon.onMobSkillCheck(nil, mob, skill) == 0)
        assert(cocoon.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = originalBuffMove
        assert(buff[1] == mob and buff[2] == xi.effect.DEFENSE_BOOST)
        assert(buff[3] == 100 and buff[4] == 0 and buff[5] == 180 and message == 123)
    end)
end)
