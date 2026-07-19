require('scripts/actions/mobskills/bionic_boost')

describe('Bionic Boost mob skill', function()
    it('allows use, applies the fixed Counterstance buff, and forwards its message', function()
        local bionicBoost = require('scripts/actions/mobskills/bionic_boost')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end
        assert(bionicBoost.onMobSkillCheck(nil, mob, skill) == 0)
        assert(bionicBoost.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.COUNTERSTANCE)
        xi.mobskills.mobBuffMove = originalBuffMove
        assert(buff[1] == mob and buff[2] == xi.effect.COUNTERSTANCE)
        assert(buff[3] == 15 and buff[4] == 1 and buff[5] == 60 and message == 123)
    end)
end)
