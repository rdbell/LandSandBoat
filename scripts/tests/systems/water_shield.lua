require('scripts/actions/mobskills/water_shield')

describe('Water Shield mob skill', function()
    it('allows use, applies the fixed Evasion Boost buff, and forwards its message', function()
        local waterShield = require('scripts/actions/mobskills/water_shield')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end
        assert(waterShield.onMobSkillCheck(nil, mob, skill) == 0)
        assert(waterShield.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.EVASION_BOOST)
        xi.mobskills.mobBuffMove = originalBuffMove
        assert(buff[1] == mob and buff[2] == xi.effect.EVASION_BOOST)
        assert(buff[3] == 20 and buff[4] == 0 and buff[5] == 30 and message == 123)
    end)
end)
