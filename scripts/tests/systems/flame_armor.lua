require('scripts/actions/mobskills/flame_armor')

describe('Flame Armor mob skill', function()
    it('allows use, applies the fixed Blaze Spikes buff, and forwards its message', function()
        local flameArmor = require('scripts/actions/mobskills/flame_armor')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end

        assert(flameArmor.onMobSkillCheck(nil, mob, skill) == 0)
        assert(flameArmor.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.BLAZE_SPIKES)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.BLAZE_SPIKES)
        assert(buff[3] == 50 and buff[4] == 0 and buff[5] == 180)
        assert(message == 123)
    end)
end)
