require('scripts/actions/mobskills/hysteroanima')
describe('Hysteroanima mob skill', function()
    it('applies MAGIC_SHIELD for 10s with USES', function()
        local skill = require('scripts/actions/mobskills/hysteroanima')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.MAGIC_SHIELD)
        assert(buff[1] == xi.effect.MAGIC_SHIELD and buff[2] == 1 and buff[4] == 10)
        assert(msg == xi.msg.basic.USES)
        xi.mobskills.mobBuffMove = buffMove
    end)
end)
