require('scripts/actions/mobskills/hundred_fists')
describe('Hundred Fists mob skill', function()
    it('applies HUNDRED_FISTS for 45s with USES', function()
        local skill = require('scripts/actions/mobskills/hundred_fists')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.HUNDRED_FISTS)
        assert(buff[1] == xi.effect.HUNDRED_FISTS and buff[2] == 1 and buff[4] == 45)
        assert(msg == xi.msg.basic.USES)
        xi.mobskills.mobBuffMove = buffMove
    end)
end)
