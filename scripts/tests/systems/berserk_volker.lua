require('scripts/actions/mobskills/berserk_volker')
describe('Berserk Volker mob skill', function()
    it('applies Warcry power 25 for 180s', function()
        local skill = require('scripts/actions/mobskills/berserk_volker')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 41
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.WARCRY)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == xi.effect.WARCRY and buff[2] == 25 and buff[4] == 180)
        assert(message == 41)
    end)
end)
