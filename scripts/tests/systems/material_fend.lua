require('scripts/actions/mobskills/material_fend')

describe('Material Fend mob skill', function()
    it('allows use, applies its fixed Evasion Boost, and forwards the buff message', function()
        local fend = require('scripts/actions/mobskills/material_fend')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end
        assert(fend.onMobSkillCheck(nil, mob, skill) == 0)
        assert(fend.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.EVASION_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == mob and buff[2] == xi.effect.EVASION_BOOST)
        assert(buff[3] == 40 and buff[4] == 0 and buff[5] == 180 and message == 123)
    end)
end)
