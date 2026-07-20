require('scripts/actions/mobskills/mighty_strikes')
describe('Mighty Strikes mob skill', function()
    it('applies MIGHTY_STRIKES for 45s with USES', function()
        local skill = require('scripts/actions/mobskills/mighty_strikes')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.MIGHTY_STRIKES)
        assert(buff[1] == xi.effect.MIGHTY_STRIKES and buff[4] == 45 and msg == xi.msg.basic.USES)
        xi.mobskills.mobBuffMove = buffMove
    end)
end)
