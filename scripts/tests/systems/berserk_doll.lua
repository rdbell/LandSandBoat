require('scripts/actions/mobskills/berserk_doll')
describe('Berserk Doll mob skill', function()
    it('applies Warcry power 33 for 120s', function()
        local skill = require('scripts/actions/mobskills/berserk_doll')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 40
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.WARCRY)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == xi.effect.WARCRY and buff[2] == 33 and buff[4] == 120)
        assert(message == 40)
    end)
end)
