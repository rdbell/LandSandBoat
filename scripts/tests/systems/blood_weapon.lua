require('scripts/actions/mobskills/blood_weapon')

describe('Blood Weapon mob skill', function()
    it('allows use, applies its fixed buff, and emits USES', function()
        local bloodWeapon = require('scripts/actions/mobskills/blood_weapon')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 999
        end

        assert(bloodWeapon.onMobSkillCheck(nil, mob, skill) == 0)
        assert(bloodWeapon.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.BLOOD_WEAPON)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.BLOOD_WEAPON)
        assert(buff[3] == 1 and buff[4] == 0 and buff[5] == 30)
        assert(message == xi.msg.basic.USES)
    end)
end)
