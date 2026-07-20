require('scripts/actions/mobskills/wind_blade_kam')
describe('Wind Blade Kam mob skill', function()
    it('applies Enaero power 65 for 30s', function()
        local skill = require('scripts/actions/mobskills/wind_blade_kam')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 55
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.ENAERO)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[2] == xi.effect.ENAERO and buff[3] == 65 and buff[4] == 0 and buff[5] == 30)
        assert(message == 55)
    end)
end)
