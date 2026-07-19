require('scripts/actions/mobskills/berserk')

describe('Berserk mob skill', function()
    it('allows use, applies the fixed Berserk buff, and forwards its message', function()
        local berserk = require('scripts/actions/mobskills/berserk')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end

        assert(berserk.onMobSkillCheck(nil, mob, skill) == 0)
        assert(berserk.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.BERSERK)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.BERSERK)
        assert(buff[3] == 45 and buff[4] == 0 and buff[5] == 120)
        assert(message == 123)
    end)
end)
