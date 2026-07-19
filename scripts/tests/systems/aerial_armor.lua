require('scripts/actions/mobskills/aerial_armor')

describe('Aerial Armor mob skill', function()
    it('allows use, applies the fixed Blink buff, and forwards its message', function()
        local aerialArmor = require('scripts/actions/mobskills/aerial_armor')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end

        assert(aerialArmor.onMobSkillCheck(nil, mob, skill) == 0)
        assert(aerialArmor.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.BLINK)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.BLINK)
        assert(buff[3] == 3 and buff[4] == 0 and buff[5] == 180)
        assert(message == 123)
    end)
end)
