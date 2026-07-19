require('scripts/actions/mobskills/amber_scutum')

describe('Amber Scutum mob skill', function()
    it('allows use, applies the fixed Defense Boost buff, and forwards its message', function()
        local amberScutum = require('scripts/actions/mobskills/amber_scutum')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end
        assert(amberScutum.onMobSkillCheck(nil, mob, skill) == 0)
        assert(amberScutum.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = originalBuffMove
        assert(buff[1] == mob and buff[2] == xi.effect.DEFENSE_BOOST)
        assert(buff[3] == 20 and buff[4] == 0 and buff[5] == 120 and message == 123)
    end)
end)
